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
			ast::asset_handle<material> mat = renderer.material;

			renderer.material->addTexture(TextureSlot::TEXTURE4, depthTexture);
			renderer.material->addTexture(TextureSlot::TEXTURE5, depthCube);

			initializeModel(ent->id, renderer);
			renderer.dirty = false;
			auto pos = std::find(m_shaders.begin(), m_shaders.end(), mat->getShader());
			if (pos == m_shaders.end())
				m_shaders.push_back(mat->getShader());
		}

		for (auto handle : m_shaders)
		{
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
	}

	void render_stage::render(core::transform camTransf, camera& cam)
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
				initializeModel(ent->id, renderer);

				renderer.dirty = false;
				auto pos = std::find(m_shaders.begin(), m_shaders.end(), shader);
				if (pos == m_shaders.end())
				{
					m_shaders.push_back(shader);
					shader->addBuffer(pointLightBuffer);
					shader->addBuffer(directionalLightBuffer);
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
			renderer.layout.bind();
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

			renderer.layout.unbind();
			material->unbind();
			model->unbind();
		}
		m_onRender(camTransf, cam);
	}

	rsl::priority_type render_stage::priority() const { return OPAQUE_PRIORITY; }

	void render_stage::initializeModel(rsl::uint entId, mesh_renderer& renderer)
	{
		auto& model = renderer.model;
		auto meshHandle = renderer.model->meshHandle;
		auto matHandle = renderer.material;
		auto& layout = renderer.layout;

		layout.release();
		layout.initialize(1, matHandle->getShader());
		layout.bind();

		model->vertexBuffer = BufferCache::createVertexBuffer<math::vec4>(std::format("{}{}-Vertex Buffer", meshHandle->name, entId), 0, UsageType::STATICDRAW, meshHandle->vertices);
		renderer.layout.setAttributePtr(model->vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);

		model->indexBuffer = BufferCache::createIndexBuffer(std::format("{}-Index Buffer", meshHandle->name), UsageType::STATICDRAW, meshHandle->indices);

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