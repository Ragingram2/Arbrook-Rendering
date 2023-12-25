#pragma once
#include "rendering/pipeline/base/graphicsstage.hpp"
#include "rendering/components/components.hpp"
#include "core/components/transform.hpp"

namespace rythe::rendering
{
	struct clear_stage : public graphics_stage<clear_stage,core::transform>
	{
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			BufferCache::createConstantBuffer<camera_data>("CameraBuffer", SV_CAMERA, UsageType::STATICDRAW);
			BufferCache::createConstantBuffer<material_data>("MaterialBuffer", SV_MATERIALS, UsageType::STATICDRAW);
			RI->setSwapInterval(0);
			RI->setViewport(1, 0, 0, Screen_Width, Screen_Height, 0, 1);
			RI->setClearColor(0x64 / 255.0f, 0x95 / 255.0f, 0xED / 255.0f, 1.0f);
			RI->setWindOrder(WindOrder::CCW);
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			ZoneScopedN("[Renderer] Clear Stage");
			RI->clear(ClearBit::COLOR_DEPTH);
			RI->depthTest(true);
			RI->updateDepthStencil();
			RI->cullFace(CullMode::BACK);
		}

		virtual rsl::priority_type priority() override { return CLEAR_PRIORITY; }
	};
}