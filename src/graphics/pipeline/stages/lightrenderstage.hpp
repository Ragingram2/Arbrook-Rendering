#pragma once
#include <rsl/primitives>
#include <rsl/utilities>
#include <rsl/math>

#include "core/events/defaults/component_event.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/interface/definitions/framebuffer.hpp"

#define MAX_LIGHT_COUNT 9
namespace rythe::rendering
{

	inline math::mat4 ortho(float left, float right, float bottom, float top, float near_plane, float far_plane)
	{
		math::mat4 result(1.0f);
		result[0][0] = 2.0f / (right - left);
		result[1][1] = 2.0f / (top - bottom);
		result[2][2] = -1.0f / (far_plane - near_plane);
		result[3][0] = -(right + left) / (right - left);
		result[3][1] = -(top + bottom) / (top - bottom);
		result[3][2] = -near_plane / (far_plane - near_plane);
		return result;
	}

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
			float near_plane = 1.0f;
			float far_plane = 15.0f;
			math::mat4 lightProjection = ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

			ZoneScopedN("[Renderer] [Light Stage] Render");
			for (auto& ent : m_filter)
			{
				auto& transf = ent.getComponent<core::transform>();
				auto& lightComp = ent.getComponent<light>();
				switch (lightComp.type)
				{
				case LightType::DIRECTIONAL:
					lightComp.data.lightProjection = lightProjection;
					lightComp.data.lightView = math::lookAt(math::vec3::zero, -transf.forward(), transf.up());
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

		virtual rsl::priority_type priority() const override { return LIGHT_PRIORITY; }

	};

}