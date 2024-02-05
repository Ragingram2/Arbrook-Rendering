#pragma once
#include <rsl/math>

#include "graphics/cache/texturecache.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/components/components.hpp"
#include "core/components/transform.hpp"


namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	struct clear_stage : public graphics_stage<clear_stage, core::transform>
	{
		framebuffer* fbo;
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			RI->makeCurrent();
			BufferCache::createConstantBuffer<camera_data>("CameraBuffer", SV_CAMERA, UsageType::STATICDRAW);
			BufferCache::createConstantBuffer<material_data>("MaterialBuffer", SV_MATERIALS, UsageType::STATICDRAW);
			RI->setSwapInterval(0);
			RI->setViewport(1, 0, 0, Screen_Width, Screen_Height, 0, 1);
			RI->setWindOrder(WindOrder::CCW);

			fbo = addFramebuffer("RenderBuffer");
			fbo->initialize();
			fbo->bind();

			texture_handle colorHandle = TextureCache::createTexture("colorAttachment", TargetType::RENDER_TARGET, { 0, nullptr }, math::ivec2(Screen_Width, Screen_Height), texture_parameters
				{
					.usage = rendering::UsageType::DEFAULT
				});

			colorHandle->bind(COLOR_SLOT);
			colorHandle->setMinFilterMode(rendering::FilterMode::LINEAR);
			colorHandle->setMagFilterMode(rendering::FilterMode::LINEAR);
			colorHandle->unbind(COLOR_SLOT);

			texture_handle depthHandle = TextureCache::createTexture("depthAttachment", TargetType::DEPTH_STENCIL, { 0, nullptr }, math::ivec2(Screen_Width, Screen_Height), texture_parameters
				{
					.format = rendering::FormatType::D24_S8,
					.usage = rendering::UsageType::DEPTH_COMPONENT
				});

			fbo->attach(AttachmentSlot::DEPTH_STENCIL, depthHandle);
			fbo->attach(AttachmentSlot::COLOR0, colorHandle);
			fbo->unbind();
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			ZoneScopedN("[Renderer] Clear Stage");
			fbo->bind();
			fbo->getAttachment(AttachmentSlot::DEPTH_STENCIL)->bind(DEPTH_STENCIL_SLOT);
			RI->checkError();
			fbo->getAttachment(AttachmentSlot::COLOR0)->bind(COLOR_SLOT);
			RI->checkError();
			RI->depthTest(true);
			RI->cullFace(CullMode::BACK);
			RI->updateDepthStencil();

			RI->setClearColor(0x64 / 255.0f, 0x95 / 255.0f, 0xED / 255.0f, 1.0f);
			RI->clear(ClearBit::COLOR_DEPTH);
		}

		virtual rsl::priority_type priority() const override { return CLEAR_PRIORITY; }
	};
}