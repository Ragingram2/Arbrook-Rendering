#pragma once
#include "graphics/pipeline/postprocessingfx/postprocessingeffect.hpp"
#include "graphics/pipeline/stages/renderstage.hpp"

namespace rythe::rendering
{

	struct test_ppfx : public post_processing_effect<test_ppfx>
	{
		framebuffer* depthFBO;
		virtual void setup(PipelineBase* pipeline, RenderInterface* RI) override
		{
			depthFBO = pipeline->getFramebuffer("DepthBuffer");
		}

		virtual void render(PipelineBase* pipeline, RenderInterface* RI, core::transform camTransf, camera& cam) override
		{
			RI->depthTest(true);
			RI->cullFace(CullMode::BACK);
			RI->updateDepthStencil();

			auto depthTexture = depthFBO->getAttachment(AttachmentSlot::DEPTH_STENCIL);
			auto depth = ShaderCache::getShader("depth_shader");
			depth->addBuffer(BufferCache::getBuffer("CameraBuffer"));
			depth->addBuffer(BufferCache::getBuffer("LightBuffer"));
			depth->bind();
			for (auto& [prio, pass] : renderPasses)
			{
				RI->setViewport(1, 0, 0, 1024, 1024);
				depthFBO->bind();

				depthTexture->bind(DEPTH_STENCIL_SLOT);
				RI->setClearColor(0x64 / 255.0f, 0x95 / 255.0f, 0xED / 255.0f, 1.0f);
				RI->clear(ClearBit::DEPTH);

				pass.invoke(camTransf, cam);

				depthTexture->unbind(DEPTH_STENCIL_SLOT);
				depthFBO->unbind();

				RI->setViewport(1, 0, 0, Screen_Width, Screen_Height);
			}
			depth->unbind();
		}

		virtual rsl::priority_type priority() const override { return 0; }
	};
}