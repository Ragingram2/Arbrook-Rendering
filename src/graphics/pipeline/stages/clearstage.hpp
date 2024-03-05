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
		framebuffer* mainFBO;
		texture_handle colorHandle;
		texture_handle mainDepthHandle;
		framebuffer* depthFBO;
		framebuffer* depthCubeFBO;
		texture_handle depthHandle;
		texture_handle depthCubeMap;
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			RI->makeCurrent();
			BufferCache::createConstantBuffer<camera_data>("CameraBuffer", SV_CAMERA, UsageType::STATICDRAW);
			BufferCache::createConstantBuffer<material_data>("MaterialBuffer", SV_MATERIALS, UsageType::STATICDRAW);
			RI->setSwapInterval(0);
			RI->setViewport(1, 0, 0, Screen_Width, Screen_Height);
			RI->setWindOrder(WindOrder::CCW);

			{
				depthFBO = addFramebuffer("DepthBuffer");
				depthFBO->initialize();
				depthFBO->bind();

				auto colorHandle = TextureCache::createTexture("shadowMap-color", TextureType::RENDER_TARGET, { 0, nullptr }, math::ivec2(Shadow_Width, Shadow_Height), texture_parameters
					{
						.usage = rendering::UsageType::DEFAULT,
						.minFilterMode = rendering::FilterMode::LINEAR,
						.magFilterMode = rendering::FilterMode::LINEAR
					});


				depthHandle = TextureCache::createTexture("shadowMap-depth", TextureType::DEPTH_STENCIL, { 0, nullptr }, math::ivec2(Shadow_Width, Shadow_Height), texture_parameters
					{
						.format = rendering::FormatType::R24_G8,
						.usage = rendering::UsageType::DEPTH_COMPONENT,
						.minFilterMode = rendering::FilterMode::NEAREST,
						.magFilterMode = rendering::FilterMode::NEAREST,
						.wrapModeS = rendering::WrapMode::CLAMP_TO_BORDER,
						.wrapModeT = rendering::WrapMode::CLAMP_TO_BORDER,
						.wrapModeR = rendering::WrapMode::CLAMP_TO_BORDER,
						.borderColor = math::vec4(1.0f)
					});

				depthFBO->attach(AttachmentSlot::COLOR0, colorHandle, true, true);
				depthFBO->attach(AttachmentSlot::DEPTH_STENCIL, depthHandle, false, false);
				depthFBO->unbind();
			}

			{
				depthCubeFBO = addFramebuffer("DepthCubeBuffer");
				depthCubeFBO->initialize();
				depthCubeFBO->bind();


				auto colorHandle = TextureCache::createTexture("shadowCube-color", TextureType::RENDER_TARGET, { 0, nullptr }, math::ivec2(Shadow_Width, Shadow_Height), texture_parameters
					{
						.usage = rendering::UsageType::DEFAULT,
						.minFilterMode = rendering::FilterMode::LINEAR,
						.magFilterMode = rendering::FilterMode::LINEAR
					});

				depthCubeMap = TextureCache::createCubemap("shadowCube-depth", { 0, nullptr }, math::ivec2(Shadow_Width, Shadow_Height), texture_parameters
					{
						.format = rendering::FormatType::R24_G8,
						.usage = rendering::UsageType::DEPTH_COMPONENT,
						.minFilterMode = rendering::FilterMode::NEAREST,
						.magFilterMode = rendering::FilterMode::NEAREST,
						.wrapModeS = rendering::WrapMode::CLAMP_TO_BORDER,
						.wrapModeT = rendering::WrapMode::CLAMP_TO_BORDER,
						.wrapModeR = rendering::WrapMode::CLAMP_TO_BORDER,
						.borderColor = math::vec4(1.0f)
					});

				//depthCubeFBO->attach(AttachmentSlot::COLOR0, colorHandle, true, true);
				depthCubeFBO->attach(AttachmentSlot::DEPTH_STENCIL, depthCubeMap, false, false);
				depthCubeFBO->unbind();
			}

			{
				mainFBO = addFramebuffer("MainBuffer");
				mainFBO->initialize();
				mainFBO->bind();

				texture_handle colorHandle = TextureCache::createTexture("main-color", TextureType::RENDER_TARGET, { 0, nullptr }, math::ivec2(Screen_Width, Screen_Height), texture_parameters
					{
						.usage = rendering::UsageType::DEFAULT,
						.minFilterMode = rendering::FilterMode::LINEAR,
						.magFilterMode = rendering::FilterMode::LINEAR
					});

				mainDepthHandle = TextureCache::createTexture("main-depth", TextureType::DEPTH_STENCIL, { 0, nullptr }, math::ivec2(Screen_Width, Screen_Height), texture_parameters
					{
						.format = rendering::FormatType::R24_G8,
						.usage = rendering::UsageType::DEPTH_COMPONENT,
						.minFilterMode = rendering::FilterMode::NEAREST,
						.magFilterMode = rendering::FilterMode::NEAREST,
						.wrapModeS = rendering::WrapMode::REPEAT,
						.wrapModeT = rendering::WrapMode::REPEAT
					});

				mainFBO->attach(AttachmentSlot::COLOR0, colorHandle, true, true);
				mainFBO->attach(AttachmentSlot::DEPTH_STENCIL, mainDepthHandle, true, true);
				mainFBO->unbind();
			}
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			ZoneScopedN("[Renderer] [Clear Stage] Render");

			mainFBO->bind();
			RI->setViewport(1, 0, 0, Screen_Width, Screen_Height);
			RI->depthTest(true);
			RI->depthWrite(true);
			RI->setDepthFunction(DepthFuncs::LESS);
			RI->updateDepthStencil();
			RI->cullFace(CullMode::BACK);

			RI->setClearColor(0x64 / 255.0f, 0x95 / 255.0f, 0xED / 255.0f, 1.0f);
			RI->clear(true, DepthClearBit::DEPTH);
		}

		virtual rsl::priority_type priority() const override { return CLEAR_PRIORITY; }
	};
}