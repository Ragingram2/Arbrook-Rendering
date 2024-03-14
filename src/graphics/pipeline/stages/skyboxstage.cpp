#include "graphics/pipeline/stages/skyboxstage.hpp"

namespace rythe::rendering
{
	void skybox_stage::setup(core::transform camTransf, camera& cam)
	{
		skyboxMat = MaterialCache::loadMaterial("skybox");
		cubeHandle = ModelCache::getModel("cube");
		cameraBuffer = BufferCache::getBuffer("CameraBuffer");
		skyboxMat->getShader()->addBuffer(cameraBuffer);

		for (auto& ent : m_filter)
		{
			skyboxMat->addTexture(TextureSlot::TEXTURE0, ent.getComponent<skybox_renderer>().skyboxTex);
			initializeSkyboxModel(ent->id, skyboxMat);
		}
	}

	void skybox_stage::render(core::transform camTransf, camera& cam)
	{
		ZoneScopedN("[Renderer] [Skybox Stage] Render");
		if (m_filter.size() < 1)
			return;

		camera_data data{ .viewPosition = camTransf.position,.projection = cam.projection,.view = cam.view, .model = math::mat4(1.0f) };

		RI->setDepthFunction(DepthFuncs::LESS_EQUAL);
		RI->updateDepthStencil();
		RI->cullFace(CullMode::FRONT);
		cameraBuffer->bufferData(&data, 1);
		skyboxMat->bind();
		cubeHandle->bind();
		layout.bind();
		for (auto submesh : cubeHandle->meshHandle->meshes)
		{
			RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
			WindowProvider::activeWindow->checkError();
		}
		layout.unbind();
		cubeHandle->unbind();
		skyboxMat->unbind();
		RI->setDepthFunction(DepthFuncs::LESS);
		RI->updateDepthStencil();
		RI->cullFace(CullMode::BACK);
	}

	 rsl::priority_type skybox_stage::priority() const { return SKYBOX_PRIORITY; }

	void skybox_stage::initializeSkyboxModel(rsl::uint entId, ast::asset_handle<material> mat)
	{
		auto& meshHandle = cubeHandle->meshHandle;
		auto& matHandle = mat;

		layout.release();
		layout.initialize(1, matHandle->getShader());
		layout.bind();

		cubeHandle->vertexBuffer = BufferCache::createVertexBuffer<math::vec4>(std::format("{}{}-Vertex Buffer", meshHandle->name, entId), 0, UsageType::STATICDRAW, meshHandle->vertices);
		layout.setAttributePtr(cubeHandle->vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);
		cubeHandle->indexBuffer = BufferCache::createIndexBuffer(std::format("{}{}-Index Buffer", meshHandle->name, entId), UsageType::STATICDRAW, meshHandle->indices);

		cubeHandle->normalBuffer = BufferCache::createVertexBuffer<math::vec3>(std::format("{}{}-Normal Buffer", meshHandle->name, entId), 1, UsageType::STATICDRAW, meshHandle->normals);
		layout.setAttributePtr(cubeHandle->normalBuffer, "NORMAL", 0, FormatType::RGB32F, 1, sizeof(math::vec3), 0);

		cubeHandle->uvBuffer = BufferCache::createVertexBuffer<math::vec2>(std::format("{}{}-UV Buffer", meshHandle->name, entId), 2, UsageType::STATICDRAW, meshHandle->texCoords);
		layout.setAttributePtr(cubeHandle->uvBuffer, "TEXCOORD", 0, FormatType::RG32F, 2, sizeof(math::vec2), 0);
		layout.submitAttributes();
	}

}