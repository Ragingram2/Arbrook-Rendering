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
		framebuffer* depthFBO;
		framebuffer* mainFBO;
		shader_handle depthShader;

		buffer_handle cameraBuffer;
		buffer_handle lightBuffer;
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			depthFBO = getFramebuffer("DepthBuffer");
			mainFBO = getFramebuffer("MainBuffer");
			cameraBuffer = BufferCache::getBuffer("CameraBuffer");
			lightBuffer = BufferCache::getBuffer("LightBuffer");
			depthShader = ShaderCache::getShader("depth_shader");
			depthShader->addBuffer(cameraBuffer);
			depthShader->addBuffer(lightBuffer);

			for (auto& ent : m_filter)
			{
				auto& renderer = ent.getComponent<mesh_renderer>();
				initializeModel(renderer.model,renderer.model->meshHandle);
			}
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			ZoneScopedN("[Renderer] [Shadow Map Stage] Render");
			mainFBO->unbind();
			depthFBO->bind();
			RI->clear(ClearBit::DEPTH);
			RI->cullFace(CullMode::FRONT);
			depthShader->bind();
			auto depthTexture = depthFBO->getAttachment(AttachmentSlot::DEPTH_STENCIL);
			depthTexture->bind(DEPTH_STENCIL_SLOT);
			camera_data data[] = { camera_data{.viewPosition = camTransf.position, .projection = cam.projection, .view = cam.view, .model = math::mat4(1.0f)} };
			for (auto& ent : m_filter)
			{
				auto& renderer = ent.getComponent<mesh_renderer>();
				if (!renderer.castShadows) continue;

				shader_handle shader = renderer.material->shader;
				ast::asset_handle<model> model = renderer.model;
				ast::asset_handle<mesh> mesh = model->meshHandle;

				if (renderer.dirty)
					initializeModel(model, mesh);

				auto& transf = ent.getComponent<core::transform>();

				data[0].model = transf.to_world();
				depthShader->setUniform("CameraBuffer", data);

				model->bind();
				if (model->indexBuffer != nullptr)
					for (unsigned int i = 0; i < mesh->meshes.size(); i++)
					{
						auto& submesh = mesh->meshes[i];
						RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
					}
				else
					RI->drawArrays(PrimitiveType::TRIANGLESLIST, 0, mesh->vertices.size());
			}

			depthShader->unbind();
			depthTexture->unbind(DEPTH_STENCIL_SLOT);
			depthFBO->unbind();
			RI->cullFace(CullMode::BACK);
			RI->clear(ClearBit::COLOR_DEPTH);
			mainFBO->bind();
		}

		void initializeModel(ast::asset_handle<model> model, ast::asset_handle<mesh> mesh)
		{
			auto meshHandle = model->meshHandle = mesh;
			auto& layout = model->layout;

			layout.release();
			layout.initialize(1, depthShader);
			layout.bind();

			model->vertexBuffer = BufferCache::createVertexBuffer<math::vec4>(std::format("{}-Depth-Vertex Buffer", meshHandle->name), 0, UsageType::STATICDRAW, meshHandle->vertices);
			model->layout.setAttributePtr(model->vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);

			model->indexBuffer = BufferCache::createIndexBuffer(std::format("{}-Depth-Index Buffer", meshHandle->name), UsageType::STATICDRAW, meshHandle->indices);
			layout.submitAttributes();
		}

		virtual rsl::priority_type priority() const override { return OPAQUE_PRIORITY - 5; }
	};
}