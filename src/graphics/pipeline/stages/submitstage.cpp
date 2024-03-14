#include "graphics/pipeline/stages/submitstage.hpp"

namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	void submit_stage::setup(core::transform camTransf, camera& cam)
	{
		screenShader = ShaderCache::getShader("screen");
		screenQuad = ModelCache::getModel("screenquad");

		layout.initialize(1, screenShader);
		layout.bind();
		screenQuad->vertexBuffer = BufferCache::createVertexBuffer<math::vec4>("ScreenQuad-Vertex Buffer", 0, UsageType::STATICDRAW, screenQuad->meshHandle->vertices);
		layout.setAttributePtr(screenQuad->vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);

		screenQuad->indexBuffer = BufferCache::createIndexBuffer("ScreenQuad-Index Buffer", UsageType::STATICDRAW, screenQuad->meshHandle->indices);

		screenQuad->uvBuffer = BufferCache::createVertexBuffer<math::vec2>("ScreenQuad-UV Buffer", 1, UsageType::STATICDRAW, screenQuad->meshHandle->texCoords);
		layout.setAttributePtr(screenQuad->uvBuffer, "TEXCOORD", 0, FormatType::RG32F, 1, sizeof(math::vec2), 0);

		layout.submitAttributes();

		mainFBO = getFramebuffer("MainBuffer");
	}

	void submit_stage::render(core::transform camTransf, camera& cam)
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
		layout.bind();
		RI->drawIndexed(PrimitiveType::TRIANGLESLIST, screenQuad->meshHandle->indexCount, 0, 0);
		WindowProvider::activeWindow->checkError();
		layout.unbind();
		screenQuad->unbind();
		colorTexture->unbind(TextureSlot::TEXTURE0);
		screenShader->unbind();
	}

	rsl::priority_type submit_stage::priority() const { return SUBMIT_PRIORITY; }
}