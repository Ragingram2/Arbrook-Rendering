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

		for (auto& ent : m_filter)
		{
			auto& renderer = ent.getComponent<mesh_renderer>();
			if (!renderer.enabled) continue;
			if (!renderer.castShadows) continue;
			initializeModel(renderer, dirShadowMap);
		}
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
		auto fboRes = mainFBO->getAttachment(AttachmentSlot::COLOR0)->getImpl().resolution;
		RI->setViewport(1, 0, 0, fboRes.x, fboRes.y);
		RI->cullFace(CullMode::BACK);
	}

	rsl::priority_type shadow_map_stage::priority() const { return OPAQUE_PRIORITY - 5; }

	void shadow_map_stage::directionalLightPass(core::transform& camTransf, camera& cam)
	{

		camera_data data[] = { camera_data{.viewPosition = camTransf.position, .projection = cam.projection, .view = cam.view, .model = math::mat4(1.0f)} };
		for (auto& ent : m_filter)
		{
			auto& renderer = ent.getComponent<mesh_renderer>();
			if (!renderer.enabled) continue;
			if (!renderer.castShadows) continue;

			model& model = renderer.model;
			ast::asset_handle<mesh> mesh = model.meshHandle;

			if (renderer.dirty)
				initializeModel(renderer, dirShadowMap);

			auto& transf = ent.getComponent<core::transform>();

			data[0].model = transf.to_world();
			dirShadowMap->setUniform("CameraBuffer", SV_CAMERA, data);
			dirShadowMap->bind();
			renderer.layout.bind();
			model.bind();
			if (model.indexBuffer != nullptr)
			{
				for (unsigned int i = 0; i < mesh->meshes.size(); i++)
				{
					auto& submesh = mesh->meshes[i];
					RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
				}
			}
			else
			{
				RI->drawArrays(PrimitiveType::TRIANGLESLIST, 0, mesh->vertices.size());
			}
			model.unbind();
			renderer.layout.unbind();
			dirShadowMap->unbind();
		}

	}

	void shadow_map_stage::pointLightPass(core::transform& camTransf, camera& cam)
	{
		for (int i = 0; i < lightInfo.count; i++)
		{
			RI->clear(false, DepthClearBit::DEPTH_STENCIL);

			lightInfo.index = i;
			pointShadowMap->setUniform("LightInfo", SV_MATERIALS + 1, &lightInfo);
			camera_data data[] = { camera_data{.viewPosition = camTransf.position, .projection = cam.projection, .view = cam.view, .model = math::mat4(1.0f)} };
			for (auto& ent : m_filter)
			{
				auto& renderer = ent.getComponent<mesh_renderer>();
				if (!ent->enabled || !renderer.enabled) continue;
				if (!renderer.castShadows) continue;

				model& model = renderer.model;
				ast::asset_handle<mesh> mesh = model.meshHandle;

				if (renderer.dirty)
					initializeModel(renderer, pointShadowMap);

				auto& transf = ent.getComponent<core::transform>();

				data[0].model = transf.to_world();
				pointShadowMap->setUniform("CameraBuffer", SV_CAMERA, data);
				pointShadowMap->bind();
				renderer.layout.bind();
				model.bind();
				if (model.indexBuffer != nullptr)
				{
					for (unsigned int i = 0; i < mesh->meshes.size(); i++)
					{
						auto& submesh = mesh->meshes[i];
						RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
					}
				}
				else
				{
					RI->drawArrays(PrimitiveType::TRIANGLESLIST, 0, mesh->vertices.size());
				}

				renderer.layout.unbind();
				model.unbind();
			}
			pointShadowMap->unbind();
		}
	}

	void shadow_map_stage::initializeModel(mesh_renderer& renderer, shader_handle handle)
	{
		auto& model = renderer.model;
		auto& layout = renderer.layout;

		layout.release();
		layout.initialize(1, handle);
		layout.bind();

		//model.vertexBuffer = BufferCache::createVertexBuffer<math::vec4>(std::format("{}-Vertex Buffer", model.meshHandle->name), 0, UsageType::STATICDRAW, model.meshHandle->vertices);
		//model.indexBuffer = BufferCache::createIndexBuffer(std::format("{}-Index Buffer", model.meshHandle->name), UsageType::STATICDRAW, model.meshHandle->indices);

		//if (model.meshHandle->normals.size() > 0)
		//	model.normalBuffer = BufferCache::createVertexBuffer<math::vec3>(std::format("{}-Normal Buffer", model.meshHandle->name), 1, UsageType::STATICDRAW, model.meshHandle->normals);

		//if (model.meshHandle->texCoords.size() > 0)
		//	model.uvBuffer = BufferCache::createVertexBuffer<math::vec2>(std::format("{}-UV Buffer", model.meshHandle->name), 2, UsageType::STATICDRAW, model.meshHandle->texCoords);

		//if (model.meshHandle->tangents.size() > 0)
		//	model.tangentBuffer = BufferCache::createVertexBuffer<math::vec3>(std::format("{}-Tangent Buffer", model.meshHandle->name), 3, UsageType::STATICDRAW, model.meshHandle->tangents);

		layout.setAttributePtr(model.vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);

		if (model.normalBuffer)
			layout.setAttributePtr(model.normalBuffer, "NORMAL", 0, FormatType::RGB32F, 1, sizeof(math::vec3), 0);

		if (model.uvBuffer)
			layout.setAttributePtr(model.uvBuffer, "TEXCOORD", 0, FormatType::RG32F, 2, sizeof(math::vec2), 0);

		if (model.tangentBuffer)
			layout.setAttributePtr(model.tangentBuffer, "TANGENT", 0, FormatType::RGB32F, 3, sizeof(math::vec3), 0);

		layout.submitAttributes();
	}

}