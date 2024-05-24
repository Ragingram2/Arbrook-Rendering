#include "graphics/pipeline/stages/skyboxstage.hpp"

namespace rythe::rendering
{
	void skybox_stage::setup(core::transform camTransf, camera& cam)
	{
		skyboxMat = MaterialCache::loadMaterial("skybox");
		cubeHandle = ModelCache::getModel("cube");
		cameraBuffer = BufferCache::getBuffer("CameraBuffer");
		skyboxMat->getShader()->addBuffer(cameraBuffer);


		cubeHandle.vertexBuffer = BufferCache::createVertexBuffer<math::vec4>(std::format("{}-Vertex Buffer", cubeHandle.meshHandle->name), 0, UsageType::STATICDRAW, cubeHandle.meshHandle->vertices);

		cubeHandle.indexBuffer = BufferCache::createIndexBuffer(std::format("{}-Index Buffer", cubeHandle.meshHandle->name), UsageType::STATICDRAW, cubeHandle.meshHandle->indices);

		cubeHandle.uvBuffer = BufferCache::createVertexBuffer<math::vec2>(std::format("{}-UV Buffer", cubeHandle.meshHandle->name), 2, UsageType::STATICDRAW, cubeHandle.meshHandle->texCoords);

		for (auto& ent : m_filter)
		{
			auto renderer = ent.getComponent<skybox_renderer>();
			if (!renderer.enabled) return;
			skyboxMat->addTexture(TextureSlot::TEXTURE0, renderer.skyboxTex);

			initializeSkyboxModel();
		}
	}

	void skybox_stage::render(core::transform camTransf, camera& cam)
	{
		ZoneScopedN("[Renderer] [Skybox Stage] Render");
		if (m_filter.size() < 1)
			return;
		WindowProvider::activeWindow->checkError();
		for (auto& ent : m_filter)
		{
			auto renderer = ent.getComponent<skybox_renderer>();
			if (!renderer.enabled) return;
		}

		camera_data data{ .viewPosition = camTransf.position,.projection = cam.projection,.view = cam.view, .model = math::mat4(1.0f) };
		RI->setDepthFunction(DepthFuncs::LESS_EQUAL);
		RI->updateDepthStencil();
		RI->cullFace(CullMode::FRONT);
		skyboxMat->setUniform("CameraBuffer", &data, SV_CAMERA);
		skyboxMat->bind();
		layout.bind();
		cubeHandle.bind();
		for (auto submesh : cubeHandle.meshHandle->meshes)
		{
			RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
			WindowProvider::activeWindow->checkError();
		}
		layout.unbind();
		skyboxMat->unbind();
		cubeHandle.unbind();

		RI->setDepthFunction(DepthFuncs::LESS);
		RI->updateDepthStencil();
		RI->cullFace(CullMode::BACK);
	}

	rsl::priority_type skybox_stage::priority() const { return SKYBOX_PRIORITY; }

	void skybox_stage::initializeSkyboxModel()
	{
		layout.release();
		layout.initialize(1, skyboxMat->getShader());
		layout.bind();

		layout.setAttributePtr(cubeHandle.vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);
		if (cubeHandle.uvBuffer)
			layout.setAttributePtr(cubeHandle.uvBuffer, "TEXCOORD", 0, FormatType::RG32F, 1, sizeof(math::vec2), 0);

		layout.submitAttributes();
	}

}