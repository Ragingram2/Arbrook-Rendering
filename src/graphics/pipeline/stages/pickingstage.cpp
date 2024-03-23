#include "graphics/pipeline/stages/pickingstage.hpp"

namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	void picking_stage::setup(core::transform camTransf, camera& cam)
	{
		mainFBO = getFramebuffer("MainBuffer");
		pickingFBO = getFramebuffer("PickingBuffer");
		entData = BufferCache::createConstantBuffer<math::ivec4>("EntityData", 1, UsageType::STATICDRAW);
		cameraBuffer = BufferCache::getBuffer("CameraBuffer");
		pickingShader = ShaderCache::getShader("picking");
		pickingShader->addBuffer(entData);
		pickingShader->addBuffer(cameraBuffer);
	}

	void picking_stage::render(core::transform camTransf, camera& cam)
	{
		mainFBO->unbind();

		pickingFBO->bind();
		RI->setClearColor(math::vec4(0.0));
		RI->clear(true, DepthClearBit::DEPTH_STENCIL);
		//RI->setViewport(1, 0, 0, Screen_Width, Screen_Height);
		camera_data data[] = { camera_data{.viewPosition = camTransf.position, .projection = cam.projection, .view = cam.view, .model = math::mat4(1.0f)} };
		for (auto& ent : m_filter)
		{
			auto vecId = convertToBase256(ent->id);
			auto& renderer = ent.getComponent<mesh_renderer>();
			auto& transf = ent.getComponent<core::transform>();
			ast::asset_handle<model> model = renderer.model;
			ast::asset_handle<mesh> mesh = renderer.model->meshHandle;

			if (renderer.dirty)
				initializeModel(ent->id, renderer);

			data[0].model = transf.to_world();
			pickingShader->setUniform("CameraBuffer", &data);
			pickingShader->setUniform("EntityData", &vecId);
			pickingShader->bind();
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

			model->unbind();
			renderer.layout.unbind();
			pickingShader->unbind();
		}
		pickingFBO->unbind();

		mainFBO->bind();
		RI->setViewport(1, 0, 0, Screen_Width, Screen_Height);
	}

	rsl::priority_type picking_stage::priority() const { return OPAQUE_PRIORITY-9; }


	void picking_stage::initializeModel(rsl::uint entId, mesh_renderer& renderer)
	{
		auto& model = renderer.model;
		auto meshHandle = renderer.model->meshHandle;
		auto& layout = renderer.layout;

		layout.release();
		layout.initialize(1, pickingShader);
		layout.bind();

		model->vertexBuffer = BufferCache::createVertexBuffer<math::vec4>(std::format("{}{}-Vertex Buffer", meshHandle->name, entId), 0, UsageType::STATICDRAW, meshHandle->vertices);
		renderer.layout.setAttributePtr(model->vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);

		model->indexBuffer = BufferCache::createIndexBuffer(std::format("{}-Index Buffer", meshHandle->name), UsageType::STATICDRAW, meshHandle->indices);

		layout.submitAttributes();
	}

	math::ivec4 picking_stage::convertToBase256(rsl::id_type id)
	{
		math::ivec4 entId = math::ivec4(0, 0, 0, 0);

		if (id <= 255)
		{
			entId.x = id;
		}
		else
		{
			rsl::id_type r = id / 256;
			id -= r * 256;
			entId.x = id;

			if (r <= 255)
			{
				entId.y = r;
			}
			else
			{
				rsl::id_type r2 = r / 256;
				r -= r2 * 256;
				entId.y = r;

				if (r2 <= 255)
				{
					entId.z = r2;
				}
				else
				{
					rsl::id_type r3 = r2 / 256;
					r2 -= r3 * 256;
					entId.z = r2;

					if (r3 <= 255)
					{
						entId.w = r3;
					}
					else
					{
						rsl::id_type r4 = r3 / 256;
						r3 -= r4 * 256;
						entId.w = r3;
					}
				}
			}
		}
		return entId;
	}
}