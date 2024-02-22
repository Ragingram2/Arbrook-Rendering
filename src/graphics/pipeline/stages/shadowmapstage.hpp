#pragma once
#include <rsl/primitives>
#include <rsl/utilities>
#include <rsl/math>
#include <rsl/logging>

#include "core/events/defaults/component_event.hpp"
#include "core/components/transform.hpp"
#include "core/assets/assethandle.hpp"
#include "graphics/cache/cache.hpp"
#include "graphics/data/shaderhandle.hpp"
#include "graphics/components/components.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/interface/definitions/definitions.hpp"

namespace rythe::rendering
{
	struct shadow_map_stage : public graphics_stage<shadow_map_stage, mesh_renderer, core::transform>
	{
		framebuffer* depthCubeFBO;
		framebuffer* depthFBO;
		framebuffer* mainFBO;
		shader_handle pointShadowMap;
		shader_handle dirShadowMap;

		buffer_handle cameraBuffer;
		buffer_handle materialBuffer;
		buffer_handle lightBuffer;
		buffer_handle lightInfoBuffer;
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			depthCubeFBO = getFramebuffer("DepthCubeBuffer");
			depthFBO = getFramebuffer("DepthBuffer");
			mainFBO = getFramebuffer("MainBuffer");

			cameraBuffer = BufferCache::getBuffer("CameraBuffer");
			lightBuffer = BufferCache::getBuffer("LightBuffer");
			materialBuffer = BufferCache::getBuffer("MaterialBuffer");
			lightInfoBuffer = BufferCache::createConstantBuffer<light_info>("LightInfo", SV_MATERIALS + 1, UsageType::STATICDRAW);

			pointShadowMap = ShaderCache::getShader("pointShadowMap");
			pointShadowMap->addBuffer(cameraBuffer);
			pointShadowMap->addBuffer(materialBuffer);
			pointShadowMap->addBuffer(lightBuffer);
			pointShadowMap->addBuffer(lightInfoBuffer);

			dirShadowMap = ShaderCache::getShader("dirShadowMap");
			dirShadowMap->addBuffer(cameraBuffer);
			dirShadowMap->addBuffer(materialBuffer);
			dirShadowMap->addBuffer(lightBuffer);
			dirShadowMap->addBuffer(lightInfoBuffer);
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			ZoneScopedN("[Renderer] [Shadow Map Stage] Render");
			mainFBO->unbind();

			RI->setViewport(1, 0, 0, Shadow_Width, Shadow_Height);
			RI->clear(ClearBit::DEPTH);
			RI->cullFace(CullMode::FRONT);

			directionalLightPass(camTransf, cam);
			pointLightPass(camTransf, cam);

			RI->cullFace(CullMode::BACK);
			RI->clear(ClearBit::COLOR_DEPTH);
			mainFBO->bind();
			RI->setViewport(1, 0, 0, Screen_Width, Screen_Height);
		}

		virtual rsl::priority_type priority() const override { return OPAQUE_PRIORITY - 5; }

		void directionalLightPass(core::transform& camTransf, camera& cam)
		{
			bindDepthMap();
			camera_data data[] = { camera_data{.viewPosition = camTransf.position, .projection = cam.projection, .view = cam.view, .model = math::mat4(1.0f)} };
			for (auto& ent : m_filter)
			{
				auto& renderer = ent.getComponent<mesh_renderer>();
				if (!renderer.castShadows) continue;

				ast::asset_handle<model> model = renderer.model;
				ast::asset_handle<mesh> mesh = model->meshHandle;

				if (renderer.dirty)
					initializeModel(model, mesh,dirShadowMap);

				auto& transf = ent.getComponent<core::transform>();

				data[0].model = transf.to_world();
				dirShadowMap->setUniform("CameraBuffer", data);

				model->bind();
				if (model->indexBuffer != nullptr)
					for (unsigned int i = 0; i < mesh->meshes.size(); i++)
					{
						auto& submesh = mesh->meshes[i];
						RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
						WindowProvider::activeWindow->checkError();
					}
				else
					RI->drawArrays(PrimitiveType::TRIANGLESLIST, 0, mesh->vertices.size());
				model->unbind();
			}
			unbindDepthMap();
		}

		void pointLightPass(core::transform& camTransf, camera& cam)
		{
			bindDepthCube();

			for (int i = 0; i < lightInfo.count; i++)
			{
				pointShadowMap->setUniform("LightInfo", &lightInfo);
				camera_data data[] = { camera_data{.viewPosition = camTransf.position, .projection = cam.projection, .view = cam.view, .model = math::mat4(1.0f)} };
				for (auto& ent : m_filter)
				{
					auto& renderer = ent.getComponent<mesh_renderer>();
					if (!renderer.castShadows) continue;

					ast::asset_handle<model> model = renderer.model;
					ast::asset_handle<mesh> mesh = model->meshHandle;

					if (renderer.dirty)
						initializeModel(model, mesh, pointShadowMap);

					auto& transf = ent.getComponent<core::transform>();

					data[0].model = transf.to_world();
					pointShadowMap->setUniform("CameraBuffer", data);

					model->bind();
					if (model->indexBuffer != nullptr)
						for (unsigned int i = 0; i < mesh->meshes.size(); i++)
						{
							auto& submesh = mesh->meshes[i];
							RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
						}
					else
						RI->drawArrays(PrimitiveType::TRIANGLESLIST, 0, mesh->vertices.size());
					model->unbind();
				}
			}
			unbindDepthCube();
		}

		void bindDepthCube()
		{
			pointShadowMap->bind();
			depthCubeFBO->bind();
			//auto depthTexture = depthCubeFBO->getAttachment(AttachmentSlot::DEPTH_STENCIL);
			//depthTexture->unbind(TextureSlot::TEXTURE2);
		}

		void unbindDepthCube()
		{
			//auto depthTexture = depthCubeFBO->getAttachment(AttachmentSlot::DEPTH_STENCIL);
			//depthTexture->unbind(TextureSlot::TEXTURE2);
			pointShadowMap->unbind();
			depthCubeFBO->unbind();
		}

		void bindDepthMap()
		{
			depthFBO->bind();
			dirShadowMap->bind();
			//auto depthTexture = depthFBO->getAttachment(AttachmentSlot::DEPTH_STENCIL);
			//depthTexture->bind(TextureSlot::TEXTURE1);
		}

		void unbindDepthMap()
		{
			//auto depthTexture = depthFBO->getAttachment(AttachmentSlot::DEPTH_STENCIL);
			//depthTexture->unbind(TextureSlot::TEXTURE1);
			dirShadowMap->unbind();
			depthFBO->unbind();
		}

		void initializeModel(ast::asset_handle<model> model, ast::asset_handle<mesh> mesh, shader_handle handle)
		{
			auto meshHandle = model->meshHandle = mesh;
			auto& layout = model->layout;

			layout.release();
			layout.initialize(1, handle);
			layout.bind();

			model->vertexBuffer = BufferCache::createVertexBuffer<math::vec4>(std::format("{}-Depth-Vertex Buffer", meshHandle->name), 0, UsageType::STATICDRAW, meshHandle->vertices);
			model->layout.setAttributePtr(model->vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);

			model->indexBuffer = BufferCache::createIndexBuffer(std::format("{}-Depth-Index Buffer", meshHandle->name), UsageType::STATICDRAW, meshHandle->indices);
			layout.submitAttributes();
		}
	};
}