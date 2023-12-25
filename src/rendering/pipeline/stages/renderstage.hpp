#pragma once
#include <rsl/primitives>
#include <rsl/utilities>
#include <rsl/math>
#include <rsl/logging>

#include "core/components/transform.hpp"
#include "rendering/cache/cache.hpp"
#include "rendering/interface/definitions/definitions.hpp"
#include "rendering/pipeline/base/graphicsstage.hpp"
#include "rendering/components/components.hpp"

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
		std::vector<shader_handle> m_shaders;
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			cam.calculate_projection();
			cameraBuffer = BufferCache::getBuffer("CameraBuffer");
			materialBuffer = BufferCache::getBuffer("MaterialBuffer");
			lightBuffer = BufferCache::getBuffer("LightBuffer");

			for (auto& ent : m_filter)
			{
				auto& renderer = ent.getComponent<mesh_renderer>();
				auto shader = renderer.material->shader;
				auto model = renderer.model;
				auto mesh = renderer.model->meshHandle;

				model->initialize(shader, mesh, renderer.instanced);
				renderer.dirty = false;
				auto pos = std::find(m_shaders.begin(), m_shaders.end(), shader);
				if (pos == m_shaders.end())
					m_shaders.push_back(shader);
			}

			for (auto handle : m_shaders)
			{
				if (lightBuffer != nullptr)
					handle->addBuffer(lightBuffer);
				if (cameraBuffer != nullptr)
					handle->addBuffer(cameraBuffer);
				if (materialBuffer != nullptr)
					handle->addBuffer(materialBuffer);
			}
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			ZoneScopedN("[Renderer] Render Stage");
			cam.calculate_view(&camTransf);
			camera_data mat = { camTransf.position, cam.projection, cam.view, math::mat4(1.0f) };
			for (auto& ent : m_filter)
			{
				auto& renderer = ent.getComponent<mesh_renderer>();
				material_handle material = renderer.material;
				shader_handle shader = renderer.material->shader;
				model_handle model = renderer.model;
				mesh_handle mesh = renderer.model->meshHandle;
				if (renderer.dirty)
				{
					model->initialize(shader, mesh, renderer.instanced);
					renderer.dirty = false;
					auto pos = std::find(m_shaders.begin(), m_shaders.end(), shader);
					if (pos == m_shaders.end())
					{
						m_shaders.push_back(shader);
						shader->addBuffer(lightBuffer);
						shader->addBuffer(cameraBuffer);
						shader->addBuffer(materialBuffer);
					}
				}
				auto& transf = ent.getComponent<core::transform>();
				mat.model = transf.to_world();
				cameraBuffer->bufferData(&mat, 1);
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
			}
			m_onRender(camTransf, cam);

			RI->checkError();
		}

		virtual rsl::priority_type priority() override { return OPAQUE_PRIORITY; }

		template <class T, void(T::* Func)(core::transform, camera)>
		static void addRender(T* ptr)
		{
			m_onRender.push_back<T, Func>(*ptr);
		}
	};

	inline rsl::multicast_delegate<renderFunc> render_stage::m_onRender;
}