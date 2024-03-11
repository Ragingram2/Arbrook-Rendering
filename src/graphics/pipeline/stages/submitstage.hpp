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
		framebuffer* mainFBO;
		framebuffer* depthFBO;
		shader_handle screenShader;
		ast::asset_handle<model> screenQuad;
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			screenShader = ShaderCache::getShader("screen");
			screenQuad = ModelCache::getModel("plane");

			screenQuad->layout.initialize(1, screenShader);
			screenQuad->layout.bind();
			screenQuad->vertexBuffer = BufferCache::createVertexBuffer<math::vec4>("ScreenQuad-Vertex Buffer", 0, UsageType::STATICDRAW, screenQuad->meshHandle->vertices);
			screenQuad->layout.setAttributePtr(screenQuad->vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);

			screenQuad->indexBuffer = BufferCache::createIndexBuffer("ScreenQuad-Index Buffer", UsageType::STATICDRAW, screenQuad->meshHandle->indices);

			screenQuad->uvBuffer = BufferCache::createVertexBuffer<math::vec2>("ScreenQuad-UV Buffer", 1, UsageType::STATICDRAW, screenQuad->meshHandle->texCoords);
			screenQuad->layout.setAttributePtr(screenQuad->uvBuffer, "TEXCOORD", 0, FormatType::RG32F, 1, sizeof(math::vec2), 0);

			screenQuad->layout.submitAttributes();

			mainFBO = getFramebuffer("MainBuffer");
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			mainFBO->unbind();
			auto colorTexture = mainFBO->getAttachment(AttachmentSlot::COLOR0);
			RI->depthTest(false);
			RI->cullFace(CullMode::NONE);
			RI->updateDepthStencil();

			RI->setClearColor(1.0f, 0.0f, 1.0f, 1.0f);
			RI->clear(true);

			screenShader->bind();
			colorTexture->bind(TextureSlot::TEXTURE0);
			screenQuad->bind();

			RI->drawIndexed(PrimitiveType::TRIANGLESLIST, screenQuad->meshHandle->indexCount, 0, 0);
			WindowProvider::activeWindow->checkError();

			colorTexture->unbind(TextureSlot::TEXTURE0);
		}

		virtual rsl::priority_type priority() const override { return SUBMIT_PRIORITY; }
	};
}
