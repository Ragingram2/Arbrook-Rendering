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
			if (!ent->enabled || !renderer.enabled) continue;
			initializeModel(renderer);
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
			if (!ent->enabled || !renderer.enabled) continue;
			model& model = renderer.model;
			ast::asset_handle<mesh> mesh = renderer.model.meshHandle;

			if (renderer.dirty)
			{
				initializeModel(renderer);
				renderer.dirty = false;
			}

			auto& transf = ent.getComponent<core::transform>();
			data[0].model = transf.to_world();

			renderer.layout.bind();
			model.bind();
			if (model.indexBuffer != nullptr)
				for (unsigned int i = 0; i < mesh->meshes.size(); i++)
				{
					auto submesh = mesh->meshes[i];
					auto mat = renderer.mainMaterial;
					//this string compares gotta go at some point I think
					auto found = std::find_if(renderer.materials.begin(), renderer.materials.end(), [&mat](auto& p) {return p.second.name == mat.name; });
					if (renderer.materials.size() > 0 && found != renderer.materials.end())
					{
						material subMat = renderer.materials[submesh.materialIdx];
						subMat.setUniform("MaterialBuffer", &subMat.data, SV_MATERIALS);
						subMat.setUniform("CameraBuffer", data, SV_CAMERA);
						subMat.bind();
						RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
						WindowProvider::activeWindow->checkError();
						subMat.unbind();
					}
					else
					{
						material subMat = renderer.mainMaterial;
						subMat.setUniform("MaterialBuffer", &subMat.data, SV_MATERIALS);
						subMat.setUniform("CameraBuffer", data, SV_CAMERA);
						subMat.bind();
						RI->drawIndexed(PrimitiveType::TRIANGLESLIST, submesh.count, submesh.indexOffset, submesh.vertexOffset);
						WindowProvider::activeWindow->checkError();
						subMat.unbind();
					}
					WindowProvider::activeWindow->checkError();
				}
			else
			{
				renderer.mainMaterial.setUniform("MaterialBuffer", &renderer.mainMaterial.data, SV_MATERIALS);
				renderer.mainMaterial.setUniform("CameraBuffer", data, SV_CAMERA);
				renderer.mainMaterial.bind();
				RI->drawArrays(PrimitiveType::TRIANGLESLIST, 0, mesh->vertices.size());
				renderer.mainMaterial.unbind();
			}

			renderer.layout.unbind();
			model.unbind();
		}
		m_onRender(camTransf, cam);
	}

	rsl::priority_type render_stage::priority() const { return OPAQUE_PRIORITY; }

	void render_stage::initializeModel(mesh_renderer& renderer)
	{
		auto& model = renderer.model;
		auto meshHandle = renderer.model.meshHandle;
		auto& layout = renderer.layout;

		if (renderer.mainMaterial)
		{
			auto handle = renderer.mainMaterial.getShader();
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
			auto handle = mat.getShader();
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

		layout.release();
		layout.initialize(1, renderer.mainMaterial.getShader());
		layout.bind();

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