#pragma once
#include "core/components/transform.hpp"
#include "core/assets/assethandle.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/components/components.hpp"
#include "graphics/cache/materialcache.hpp"
#include "graphics/cache/modelcache.hpp"
#include "graphics/data/model.hpp"


namespace rythe::rendering
{
	struct skybox_stage : public graphics_stage<skybox_stage, skybox_renderer>
	{
		buffer_handle cameraBuffer;
		ast::asset_handle<material> skyboxMat;
		ast::asset_handle<model> cubeHandle;
		inputlayout layout;
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			skyboxMat = MaterialCache::loadMaterialFromFile("skybox", "resources/shaders/skybox.shader");
			cubeHandle = ModelCache::getModel("cube");
			cameraBuffer = BufferCache::getBuffer("CameraBuffer");
			skyboxMat->getShader()->addBuffer(cameraBuffer);

			for (auto& ent : m_filter)
			{
				skyboxMat->addTexture(TextureSlot::TEXTURE0, ent.getComponent<skybox_renderer>().skyboxTex);
				initializeSkyboxModel(ent->id, skyboxMat);
			}
		}

		virtual void render(core::transform camTransf, camera& cam) override
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

			for (auto submesh : cubeHandle->meshHandle->meshes)
			{
				RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
				WindowProvider::activeWindow->checkError();
			}
			skyboxMat->unbind();
			RI->setDepthFunction(DepthFuncs::LESS);
			RI->updateDepthStencil();
			RI->cullFace(CullMode::BACK);
		}

		virtual rsl::priority_type priority() const override { return SKYBOX_PRIORITY; }

		void initializeSkyboxModel(rsl::uint entId, ast::asset_handle<material> mat)
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
	};
}