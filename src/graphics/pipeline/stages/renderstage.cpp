#include "graphics/pipeline/stages/renderstage.hpp"

namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	using renderFunc = void(core::transform, camera);
	rsl::multicast_delegate<renderFunc> render_stage::m_onRender;

	void render_stage::setup(core::transform camTransf, camera& cam)
	{
		depthTexture = getFramebuffer("DepthBuffer")->getAttachment(AttachmentSlot::DEPTH_STENCIL);
		depthCube = getFramebuffer("DepthCubeBuffer")->getAttachment(AttachmentSlot::DEPTH_STENCIL);

		cameraBuffer = BufferCache::getBuffer("CameraBuffer");
		materialBuffer = BufferCache::getBuffer("MaterialBuffer");
		pointLightBuffer = BufferCache::getBuffer("PointLightBuffer");
		directionalLightBuffer = BufferCache::getBuffer("DirectionalLightBuffer");
		lightInfoBuffer = BufferCache::getBuffer("LightInfo");

		for (auto& ent : m_filter)
		{
			auto& renderer = ent.getComponent<mesh_renderer>();

			initializeModel(ent->id, renderer);
			renderer.dirty = false;
		}
	}

	void render_stage::render(core::transform camTransf, camera& cam)
	{
		ZoneScopedN("[Renderer] [Render Stage] Render");
		camera_data data[] = { camera_data{.viewPosition = camTransf.position, .projection = cam.projection, .view = cam.view, .model = math::mat4(1.0f)} };
		for (auto& ent : m_filter)
		{
			auto& renderer = ent.getComponent<mesh_renderer>();
			ast::asset_handle<model> model = renderer.model;
			ast::asset_handle<mesh> mesh = renderer.model->meshHandle;
			if (renderer.dirty)
			{
				initializeModel(ent->id, renderer);
				renderer.dirty = false;
			}
			auto& transf = ent.getComponent<core::transform>();
			data[0].model = transf.to_world();

			renderer.layout.bind();
			model->bind();
			if (model->indexBuffer != nullptr)
				for (unsigned int i = 0; i < mesh->meshes.size(); i++)
				{
					auto& submesh = mesh->meshes[i];
					auto mat = renderer.mainMaterial;
					auto found = std::find_if(renderer.materials.begin(), renderer.materials.end(), [&mat](auto& p) {return p.second == mat; });
					if (renderer.materials.size() > 0 && found != renderer.materials.end())
					{
						ast::asset_handle<material> subMat = renderer.materials[submesh.materialIdx];
						subMat->bind();
						subMat->getShader()->setUniform("MaterialBuffer", SV_MATERIALS, &subMat->data);
						subMat->getShader()->setUniform("CameraBuffer", SV_CAMERA, data);
						RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
						subMat->unbind();
					}
					else
					{
						ast::asset_handle<material> subMat = renderer.mainMaterial;
						subMat->bind();
						subMat->getShader()->setUniform("MaterialBuffer", SV_MATERIALS, &subMat->data);
						subMat->getShader()->setUniform("CameraBuffer", SV_CAMERA, data);
						RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
						subMat->unbind();
					}
					WindowProvider::activeWindow->checkError();
				}
			else
			{
				renderer.mainMaterial->bind();
				renderer.mainMaterial->getShader()->setUniform("MaterialBuffer", SV_MATERIALS, &renderer.mainMaterial->data);
				renderer.mainMaterial->getShader()->setUniform("CameraBuffer", SV_CAMERA, data);
				RI->drawArrays(PrimitiveType::TRIANGLESLIST, 0, mesh->vertices.size());
				renderer.mainMaterial->unbind();
			}

			renderer.layout.unbind();
			model->unbind();
		}
		m_onRender(camTransf, cam);
	}

	rsl::priority_type render_stage::priority() const { return OPAQUE_PRIORITY; }

	void render_stage::initializeModel(rsl::uint entId, mesh_renderer& renderer)
	{
		auto& model = renderer.model;

		auto meshHandle = renderer.model->meshHandle;
		auto matHandle = renderer.materials.size() > 0 ? renderer.materials[meshHandle->meshes[0].materialIdx] : renderer.mainMaterial;
		auto& layout = renderer.layout;

		if (renderer.mainMaterial)
		{
			auto handle = renderer.mainMaterial->getShader();
			if (pointLightBuffer != nullptr)
				handle->addBuffer(pointLightBuffer);
			if (directionalLightBuffer != nullptr)
				handle->addBuffer(directionalLightBuffer);
			if (cameraBuffer != nullptr)
				handle->addBuffer(cameraBuffer);
			if (materialBuffer != nullptr)
				handle->addBuffer(materialBuffer);
			if (lightInfoBuffer != nullptr)
				handle->addBuffer(lightInfoBuffer);

			//if (depthTexture)
			renderer.mainMaterial->addTexture(TextureSlot::TEXTURE4, depthTexture);
			//if (depthCube)
			renderer.mainMaterial->addTexture(TextureSlot::TEXTURE5, depthCube);
		}

		for (auto [id, matId] : model->meshHandle->materialIds)
		{
			renderer.materials[id] = MaterialCache::getMaterial(matId);
			auto handle = renderer.materials[id]->getShader();
			if (pointLightBuffer != nullptr)
				handle->addBuffer(pointLightBuffer);
			if (directionalLightBuffer != nullptr)
				handle->addBuffer(directionalLightBuffer);
			if (cameraBuffer != nullptr)
				handle->addBuffer(cameraBuffer);
			if (materialBuffer != nullptr)
				handle->addBuffer(materialBuffer);
			if (lightInfoBuffer != nullptr)
				handle->addBuffer(lightInfoBuffer);
		}

		for (auto [id, mat] : renderer.materials)
		{
			//if (depthTexture)
			mat->addTexture(TextureSlot::TEXTURE4, depthTexture);
			//if (depthCube)
			mat->addTexture(TextureSlot::TEXTURE5, depthCube);
		}


		layout.release();
		layout.initialize(1, matHandle->getShader());
		layout.bind();

		model->vertexBuffer = BufferCache::createVertexBuffer<math::vec4>(std::format("{}{}-Vertex Buffer", meshHandle->name, entId), 0, UsageType::STATICDRAW, meshHandle->vertices);
		renderer.layout.setAttributePtr(model->vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);

		model->indexBuffer = BufferCache::createIndexBuffer(std::format("{}{}-Index Buffer", meshHandle->name, entId), UsageType::STATICDRAW, meshHandle->indices);

		if (meshHandle->normals.size() > 0)
		{
			model->normalBuffer = BufferCache::createVertexBuffer<math::vec3>(std::format("{}{}-Normal Buffer", meshHandle->name, entId), 1, UsageType::STATICDRAW, meshHandle->normals);
			layout.setAttributePtr(model->normalBuffer, "NORMAL", 0, FormatType::RGB32F, 1, sizeof(math::vec3), 0);
		}

		if (meshHandle->texCoords.size() > 0)
		{
			model->uvBuffer = BufferCache::createVertexBuffer<math::vec2>(std::format("{}{}-UV Buffer", meshHandle->name, entId), 2, UsageType::STATICDRAW, meshHandle->texCoords);
			layout.setAttributePtr(model->uvBuffer, "TEXCOORD", 0, FormatType::RG32F, 2, sizeof(math::vec2), 0);
		}

		if (meshHandle->tangents.size() > 0)
		{
			model->tangentBuffer = BufferCache::createVertexBuffer<math::vec3>(std::format("{}{}-Tangent Buffer", meshHandle->name, entId), 3, UsageType::STATICDRAW, meshHandle->tangents);
			layout.setAttributePtr(model->tangentBuffer, "TANGENT", 0, FormatType::RGB32F, 3, sizeof(math::vec3), 0);
		}

		if (renderer.instanced)
		{
			model->matrixBuffer = BufferCache::createBuffer<math::mat4>(std::format("{}{}-Matrix Buffer", meshHandle->name, entId), BufferType::VERTEX_BUFFER);
			layout.setAttributePtr(model->matrixBuffer, "MODEL", 1, FormatType::RGBA32F, 4, sizeof(math::mat4), 0.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
			layout.setAttributePtr(model->matrixBuffer, "MODEL", 2, FormatType::RGBA32F, 4, sizeof(math::mat4), 1.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
			layout.setAttributePtr(model->matrixBuffer, "MODEL", 3, FormatType::RGBA32F, 4, sizeof(math::mat4), 2.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
			layout.setAttributePtr(model->matrixBuffer, "MODEL", 4, FormatType::RGBA32F, 4, sizeof(math::mat4), 3.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
		}
		layout.submitAttributes();
	}
}