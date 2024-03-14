#include "graphics/pipeline/stages/shadowmapstage.hpp"

namespace rythe::rendering
{
	void shadow_map_stage::setup(core::transform camTransf, camera& cam)
	{
		depthCubeFBO = getFramebuffer("DepthCubeBuffer");
		depthFBO = getFramebuffer("DepthBuffer");
		mainFBO = getFramebuffer("MainBuffer");

		cameraBuffer = BufferCache::getBuffer("CameraBuffer");
		pointLightBuffer = BufferCache::getBuffer("PointLightBuffer");
		directionalLightBuffer = BufferCache::getBuffer("DirectionalLightBuffer");
		materialBuffer = BufferCache::getBuffer("MaterialBuffer");
		lightInfoBuffer = BufferCache::createConstantBuffer<light_info>("LightInfo", SV_MATERIALS + 1, UsageType::STATICDRAW);

		pointShadowMap = ShaderCache::getShader("pointShadowMap");
		pointShadowMap->addBuffer(cameraBuffer);
		pointShadowMap->addBuffer(materialBuffer);
		pointShadowMap->addBuffer(pointLightBuffer);
		pointShadowMap->addBuffer(lightInfoBuffer);

		dirShadowMap = ShaderCache::getShader("dirShadowMap");
		dirShadowMap->addBuffer(cameraBuffer);
		dirShadowMap->addBuffer(materialBuffer);
		dirShadowMap->addBuffer(directionalLightBuffer);
		dirShadowMap->addBuffer(lightInfoBuffer);
	}

	void shadow_map_stage::render(core::transform camTransf, camera& cam)
	{
		ZoneScopedN("[Renderer] [Shadow Map Stage] Render");
		mainFBO->unbind();

		depthFBO->bind();
		RI->setViewport(1, 0, 0, Shadow_Width, Shadow_Height);
		RI->clear(false, DepthClearBit::DEPTH_STENCIL);
		RI->cullFace(CullMode::FRONT);
		directionalLightPass(camTransf, cam);
		depthFBO->unbind();

		depthCubeFBO->bind();
		RI->setViewport(1, 0, 0, Shadow_Width, Shadow_Height);
		RI->clear(false, DepthClearBit::DEPTH_STENCIL);
		RI->cullFace(CullMode::FRONT);
		pointLightPass(camTransf, cam);
		depthCubeFBO->unbind();

		mainFBO->bind();
		RI->setViewport(1, 0, 0, Screen_Width, Screen_Height);
		RI->cullFace(CullMode::BACK);
	}

	rsl::priority_type shadow_map_stage::priority() const { return OPAQUE_PRIORITY - 5; }

	void shadow_map_stage::directionalLightPass(core::transform& camTransf, camera& cam)
	{
		dirShadowMap->bind();
		camera_data data[] = { camera_data{.viewPosition = camTransf.position, .projection = cam.projection, .view = cam.view, .model = math::mat4(1.0f)} };
		for (auto& ent : m_filter)
		{
			auto& renderer = ent.getComponent<mesh_renderer>();
			if (!renderer.castShadows) continue;

			ast::asset_handle<model> model = renderer.model;
			ast::asset_handle<mesh> mesh = model->meshHandle;

			if (renderer.dirty)
				initializeModel(ent->id, renderer, dirShadowMap);

			auto& transf = ent.getComponent<core::transform>();

			data[0].model = transf.to_world();
			dirShadowMap->setUniform("CameraBuffer", data);

			model->bind();
			renderer.layout.bind();
			if (model->indexBuffer != nullptr)
				for (unsigned int i = 0; i < mesh->meshes.size(); i++)
				{
					auto& submesh = mesh->meshes[i];
					RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
				}
			else
				RI->drawArrays(PrimitiveType::TRIANGLESLIST, 0, mesh->vertices.size());
			renderer.layout.unbind();
			model->unbind();
		}
		dirShadowMap->unbind();
	}

	void shadow_map_stage::pointLightPass(core::transform& camTransf, camera& cam)
	{
		for (int i = 0; i < lightInfo.count; i++)
		{
			RI->clear(false, DepthClearBit::DEPTH_STENCIL);
			pointShadowMap->bind();
			lightInfo.index = i;
			pointShadowMap->setUniform("LightInfo", &lightInfo);
			camera_data data[] = { camera_data{.viewPosition = camTransf.position, .projection = cam.projection, .view = cam.view, .model = math::mat4(1.0f)} };
			for (auto& ent : m_filter)
			{
				auto& renderer = ent.getComponent<mesh_renderer>();
				if (!renderer.castShadows) continue;

				ast::asset_handle<model> model = renderer.model;
				ast::asset_handle<mesh> mesh = model->meshHandle;

				if (renderer.dirty)
					initializeModel(ent->id, renderer, pointShadowMap);

				auto& transf = ent.getComponent<core::transform>();

				data[0].model = transf.to_world();
				pointShadowMap->setUniform("CameraBuffer", data);
				model->bind();
				renderer.layout.bind();
				if (model->indexBuffer != nullptr)
					for (unsigned int i = 0; i < mesh->meshes.size(); i++)
					{
						auto& submesh = mesh->meshes[i];
						RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
					}
				else
					RI->drawArrays(PrimitiveType::TRIANGLESLIST, 0, mesh->vertices.size());
				renderer.layout.unbind();
				model->unbind();
			}
			pointShadowMap->unbind();
		}
	}

	void shadow_map_stage::initializeModel(rsl::uint entId, mesh_renderer& renderer, shader_handle handle)
	{
		auto meshHandle = renderer.model->meshHandle;
		auto model = renderer.model;
		auto& layout = renderer.layout;

		layout.release();
		layout.initialize(1, handle);
		layout.bind();

		model->vertexBuffer = BufferCache::createVertexBuffer<math::vec4>(std::format("{}{}-Depth-Vertex Buffer", meshHandle->name, entId), 0, UsageType::STATICDRAW, meshHandle->vertices);
		layout.setAttributePtr(model->vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);

		model->indexBuffer = BufferCache::createIndexBuffer(std::format("{}{}-Depth-Index Buffer", meshHandle->name, entId), UsageType::STATICDRAW, meshHandle->indices);
		layout.submitAttributes();
	}

}