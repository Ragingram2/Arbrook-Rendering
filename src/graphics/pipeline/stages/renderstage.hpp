#pragma once
#include <rsl/primitives>
#include <rsl/utilities>
#include <rsl/math>
#include <rsl/logging>

#include "core/components/transform.hpp"
#include "core/assets/assethandle.hpp"
#include "graphics/cache/cache.hpp"
#include "graphics/data/shaderhandle.hpp"
#include "graphics/interface/definitions/definitions.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/components/components.hpp"

namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	using renderFunc = void(core::transform, camera);
	struct render_stage : public graphics_stage<render_stage, mesh_renderer, core::transform>
	{
	private:
		static rsl::multicast_delegate<renderFunc> m_onRender;
	public:
		buffer_handle cameraBuffer;
		buffer_handle materialBuffer;
		buffer_handle lightBuffer;
		buffer_handle lightInfoBuffer;
		std::vector<shader_handle> m_shaders;

		texture_handle depthTexture;
		texture_handle depthCube;
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			depthTexture = getFramebuffer("DepthBuffer")->getAttachment(AttachmentSlot::DEPTH_STENCIL);
			depthCube = getFramebuffer("DepthCubeBuffer")->getAttachment(AttachmentSlot::DEPTH_STENCIL);

			cameraBuffer = BufferCache::getBuffer("CameraBuffer");
			materialBuffer = BufferCache::getBuffer("MaterialBuffer");
			lightBuffer = BufferCache::getBuffer("LightBuffer");
			lightInfoBuffer = BufferCache::getBuffer("LightInfo");

			for (auto& ent : m_filter)
			{
				auto& renderer = ent.getComponent<mesh_renderer>();
				ast::asset_handle<material> mat = renderer.material;

				renderer.material->addTexture(TextureSlot::TEXTURE1, depthTexture);
				renderer.material->addTexture(TextureSlot::TEXTURE2, depthCube);

				initializeModel(renderer.model, mat, renderer.model->meshHandle, renderer.instanced);
				renderer.dirty = false;
				auto pos = std::find(m_shaders.begin(), m_shaders.end(), mat->getShader());
				if (pos == m_shaders.end())
					m_shaders.push_back(mat->getShader());
			}

			for (auto handle : m_shaders)
			{
				if (lightBuffer != nullptr)
					handle->addBuffer(lightBuffer);
				if (cameraBuffer != nullptr)
					handle->addBuffer(cameraBuffer);
				if (materialBuffer != nullptr)
					handle->addBuffer(materialBuffer);
				if (lightInfoBuffer != nullptr)
					handle->addBuffer(lightInfoBuffer);
			}
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			ZoneScopedN("[Renderer] [Render Stage] Render");
			camera_data data[] = { camera_data{.viewPosition = camTransf.position, .projection = cam.projection, .view = cam.view, .model = math::mat4(1.0f)} };

			for (auto& ent : m_filter)
			{
				auto& renderer = ent.getComponent<mesh_renderer>();
				ast::asset_handle<material> material = renderer.material;
				shader_handle shader = renderer.material->getShader();
				ast::asset_handle<model> model = renderer.model;
				ast::asset_handle<mesh> mesh = renderer.model->meshHandle;
				if (renderer.dirty)
				{
					initializeModel(model, material, mesh, renderer.instanced);

					renderer.dirty = false;
					auto pos = std::find(m_shaders.begin(), m_shaders.end(), shader);
					if (pos == m_shaders.end())
					{
						m_shaders.push_back(shader);
						shader->addBuffer(lightBuffer);
						shader->addBuffer(cameraBuffer);
						shader->addBuffer(materialBuffer);
						shader->addBuffer(lightInfoBuffer);
					}
				}
				auto& transf = ent.getComponent<core::transform>();
				data[0].model = transf.to_world();
				cameraBuffer->bufferData(data, 1);
				materialBuffer->bufferData(&material->data, 1);
				material->bind();
				model->bind();
				if (model->indexBuffer != nullptr)
					for (unsigned int i = 0; i < mesh->meshes.size(); i++)
					{
						auto& submesh = mesh->meshes[i];
						RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
					}
				else
					RI->drawArrays(PrimitiveType::TRIANGLESLIST, 0, mesh->vertices.size());

				material->unbind();
			}
			m_onRender(camTransf, cam);
		}

		virtual rsl::priority_type priority() const override { return OPAQUE_PRIORITY; }

		void initializeModel(ast::asset_handle<model> model, ast::asset_handle<material> mat, ast::asset_handle<mesh> mesh, bool instanced = false)
		{
			auto meshHandle = model->meshHandle = mesh;
			auto matHandle = model->matHandle = mat;
			auto& layout = model->layout;

			layout.release();
			layout.initialize(1, matHandle->getShader());
			layout.bind();

			model->vertexBuffer = BufferCache::createVertexBuffer<math::vec4>(std::format("{}-Vertex Buffer", meshHandle->name), 0, UsageType::STATICDRAW, meshHandle->vertices);
			model->layout.setAttributePtr(model->vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);

			model->indexBuffer = BufferCache::createIndexBuffer(std::format("{}-Index Buffer", meshHandle->name), UsageType::STATICDRAW, meshHandle->indices);

			model->normalBuffer = BufferCache::createVertexBuffer<math::vec3>(std::format("{}-Normal Buffer", meshHandle->name), 1, UsageType::STATICDRAW, meshHandle->normals);
			layout.setAttributePtr(model->normalBuffer, "NORMAL", 0, FormatType::RGB32F, 1, sizeof(math::vec3), 0);

			model->uvBuffer = BufferCache::createVertexBuffer<math::vec2>(std::format("{}-UV Buffer", meshHandle->name), 2, UsageType::STATICDRAW, meshHandle->texCoords);
			layout.setAttributePtr(model->uvBuffer, "TEXCOORD", 0, FormatType::RG32F, 2, sizeof(math::vec2), 0);


			if (instanced)
			{
				model->matrixBuffer = BufferCache::createBuffer<math::mat4>(std::format("{}-Matrix Buffer", meshHandle->name), TargetType::VERTEX_BUFFER);
				layout.setAttributePtr(model->matrixBuffer, "MODEL", 1, FormatType::RGBA32F, 3, sizeof(math::mat4), 0.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
				layout.setAttributePtr(model->matrixBuffer, "MODEL", 2, FormatType::RGBA32F, 3, sizeof(math::mat4), 1.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
				layout.setAttributePtr(model->matrixBuffer, "MODEL", 3, FormatType::RGBA32F, 3, sizeof(math::mat4), 2.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
				layout.setAttributePtr(model->matrixBuffer, "MODEL", 4, FormatType::RGBA32F, 3, sizeof(math::mat4), 3.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
			}
			layout.submitAttributes();
		}

		template <class T, void(T::* Func)(core::transform, camera)>
		static void addRender(T* ptr)
		{
			m_onRender.push_back<T, Func>(*ptr);
		}

	};

	inline rsl::multicast_delegate<renderFunc> render_stage::m_onRender;
}