#pragma once
#include <rsl/primitives>
#include <rsl/utilities>
#include <rsl/math>

#include "core/events/defaults/component_event.hpp"
#include "rendering/pipeline/base/graphicsstage.hpp"
#define MAX_LIGHT_COUNT 9
namespace rythe::rendering
{
	struct light_buffer
	{
		light_data data[MAX_LIGHT_COUNT];
	};

	struct light_render_stage : public graphics_stage<light_render_stage, light>
	{
		rsl::uint m_lastIdx = 0;
		buffer_handle lightBuffer;
		light_buffer lightDataStruct;
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			lightBuffer = BufferCache::createConstantBuffer<light_buffer>("LightBuffer", SV_LIGHTS, UsageType::STATICDRAW);
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			ZoneScopedN("[Renderer] Light Stage");
			for (auto& ent : m_filter)
			{
				auto& transf = ent.getComponent<core::transform>();
				auto& lightComp = ent.getComponent<light>();
				switch (lightComp.type)
				{
				case LightType::DIRECTIONAL:
					lightComp.data.direction = transf.forward();
					lightDataStruct.data[0] = lightComp.data;
					break;
				case LightType::POINT:
					lightComp.data.position = transf.position;
					if (lightComp.index == 0)
						lightComp.index = m_lastIdx++;
					if (lightComp.index < MAX_LIGHT_COUNT)
						lightDataStruct.data[lightComp.index] = lightComp.data;
					break;
				case LightType::SPOT:
					lightComp.data.position = transf.position;
					break;
				}
			}

			if (lightBuffer != nullptr)
			{
				lightBuffer->bufferData(&lightDataStruct, 1);
			}
		}

		virtual rsl::priority_type priority() override { return LIGHT_PRIORITY; }

	};

}