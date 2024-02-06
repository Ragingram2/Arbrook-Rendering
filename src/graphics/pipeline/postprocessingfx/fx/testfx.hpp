#pragma once
#include "graphics/pipeline/postprocessingfx/postprocessingeffect.hpp"
#include "graphics/pipeline/stages/renderstage.hpp"

namespace rythe::rendering
{

	struct test_ppfx : public post_processing_effect<test_ppfx>
	{
		virtual void setup() override
		{
		}

		virtual void render(framebuffer* fbo, RenderInterface* RI, core::transform camTransf, camera& cam) override
		{
			for (auto& [prio, pass] : renderPasses)
			{

				RI->setViewport(1, 0, 0, 1024, 1024);
				fbo->bind();
				fbo->getAttachment(AttachmentSlot::DEPTH_STENCIL)->bind(DEPTH_STENCIL_SLOT);
				RI->depthTest(true);
				RI->cullFace(CullMode::BACK);
				RI->updateDepthStencil();
				RI->setClearColor(0x64 / 255.0f, 0x95 / 255.0f, 0xED / 255.0f, 1.0f);
				RI->clear(ClearBit::DEPTH);

				pass.invoke(camTransf, cam);

				fbo->getAttachment(AttachmentSlot::DEPTH_STENCIL)->unbind(DEPTH_STENCIL_SLOT);
				fbo->unbind();

				RI->setViewport(1, 0, 0, Screen_Width, Screen_Height);
			}
		}

		virtual rsl::priority_type priority() const override { return 0; }
	};
}