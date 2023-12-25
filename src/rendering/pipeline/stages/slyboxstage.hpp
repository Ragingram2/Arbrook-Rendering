#pragma once
#include "rendering/pipeline/base/graphicsstage.hpp"
#include "rendering/components/components.hpp"
#include "rendering/data/materialhandle.hpp"
#include "rendering/cache/materialcache.hpp"
#include "core/components/transform.hpp"


namespace rythe::rendering
{
	struct skybox_stage : public graphics_stage<skybox_stage, skybox_renderer>
	{
		buffer_handle cameraBuffer;
		material_handle skyboxMat;
		model_handle cubeHandle;
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			cam.calculate_projection();
			skyboxMat = MaterialCache::loadMaterialFromFile("skybox", "resources/shaders/skybox.shader");
			cubeHandle = ModelCache::getModel("sphere");
			cameraBuffer = BufferCache::getBuffer("CameraBuffer");
			cubeHandle->initialize(skyboxMat->shader, cubeHandle->meshHandle, false);
			for (auto& ent : m_filter)
			{
				skyboxMat->diffuse = ent.getComponent<skybox_renderer>().skyboxTex;
			}
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			ZoneScopedN("[Renderer] Skybox Stage");
			if (m_filter.size() < 1)
				return;

			cam.calculate_view(&camTransf);
			camera_data mat = { camTransf.position, cam.projection, cam.view, math::mat4(1.0f) };

			RI->depthTest(true);
			RI->depthWrite(true);
			RI->setDepthFunction(DepthFuncs::LESS_EQUAL);
			RI->updateDepthStencil();
			RI->cullFace(CullMode::FRONT);
			cameraBuffer->bufferData(&mat, 1);
			skyboxMat->bind();
			cubeHandle->bind();

			for (auto submesh : cubeHandle->meshHandle->meshes)
			{
				RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
			}

			RI->setDepthFunction(DepthFuncs::LESS);
			RI->updateDepthStencil();
			RI->cullFace(CullMode::BACK);
		}

		virtual rsl::priority_type priority() override { return SKYBOX_PRIORITY; }
	};
}