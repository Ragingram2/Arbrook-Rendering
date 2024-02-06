#pragma once
#include "core/assets/assethandle.hpp"
#include "core/components/transform.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/components/components.hpp"
#include "graphics/cache/shadercache.hpp"
#include "graphics/cache/modelcache.hpp"

namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	struct submit_stage : public graphics_stage<submit_stage, core::transform>
	{
		framebuffer* fbo;
		shader_handle screenShader;
		ast::asset_handle<model> screenQuad;
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			screenShader = ShaderCache::getShader("screen");
			screenQuad = ModelCache::getModel("plane");

			screenQuad->layout.initialize(1, screenShader);
			screenQuad->layout.bind();
			screenQuad->vertexBuffer = BufferCache::createVertexBuffer<math::vec4>(std::format("{}-Vertex Buffer", screenQuad->meshHandle->name), 0, UsageType::STATICDRAW, screenQuad->meshHandle->vertices);
			screenQuad->layout.setAttributePtr(screenQuad->vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);

			screenQuad->indexBuffer = BufferCache::createIndexBuffer(std::format("{}-Index Buffer", screenQuad->meshHandle->name), UsageType::STATICDRAW, screenQuad->meshHandle->indices);

			screenQuad->uvBuffer = BufferCache::createVertexBuffer<math::vec2>(std::format("{}-UV Buffer", screenQuad->meshHandle->name), 1, UsageType::STATICDRAW, screenQuad->meshHandle->texCoords);
			screenQuad->layout.setAttributePtr(screenQuad->uvBuffer, "TEXCOORD", 0, FormatType::RG32F, 1, sizeof(math::vec2), 0);

			screenQuad->layout.submitAttributes();

			fbo = getFramebuffer("RenderBuffer");
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			fbo->getAttachment(AttachmentSlot::COLOR0)->unbind(COLOR_SLOT);
			fbo->getAttachment(AttachmentSlot::DEPTH_STENCIL)->unbind(DEPTH_STENCIL_SLOT);
			fbo->unbind();
			RI->depthTest(false);
			RI->cullFace(CullMode::NONE);
			RI->updateDepthStencil();

			RI->setClearColor(1.0f, 0.0f, 1.0f, 1.0f);
			RI->clear(ClearBit::COLOR);

			fbo->getAttachment(AttachmentSlot::DEPTH_STENCIL)->bind(DEPTH_STENCIL_SLOT);
			fbo->getAttachment(AttachmentSlot::COLOR0)->bind(COLOR_SLOT);
			screenShader->bind();
			screenQuad->bind();

			RI->drawIndexed(PrimitiveType::TRIANGLESLIST, screenQuad->meshHandle->indexCount, 0, 0);

			fbo->getAttachment(AttachmentSlot::DEPTH_STENCIL)->unbind(DEPTH_STENCIL_SLOT);
			fbo->getAttachment(AttachmentSlot::COLOR0)->unbind(COLOR_SLOT);
		}

		virtual rsl::priority_type priority() const override { return SUBMIT_PRIORITY; }
	};
}
