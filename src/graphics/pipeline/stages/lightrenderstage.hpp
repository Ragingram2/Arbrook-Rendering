#pragma once
#include <rsl/primitives>
#include <rsl/utilities>
#include <rsl/math>

#include "core/events/defaults/component_event.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/interface/definitions/definitions.hpp"

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
		point_light_data point_data[MAX_POINT_LIGHT_COUNT];
		dir_light_data dir_data[1];
	};

	struct light_render_stage : public graphics_stage<light_render_stage, light>
	{
		rsl::uint m_lastIdx = 0;
		buffer_handle lightBuffer;
		light_buffer lightDataStruct;

		math::mat4 lightProjection;
		math::mat4 shadowProjection;
		std::vector<math::mat4> shadowTransforms;

		float near_plane = 1.0f;
		float far_plane = 50.0f;

		virtual void setup(core::transform camTransf, camera& cam) override
		{
			lightBuffer = BufferCache::createConstantBuffer<light_buffer>("LightBuffer", SV_LIGHTS, UsageType::STATICDRAW);

			lightProjection = ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

			shadowProjection = math::perspective(math::radians(90.0f), Shadow_Width / Shadow_Height, near_plane, far_plane);
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{

			ZoneScopedN("[Renderer] [Light Stage] Render");
			for (auto& ent : m_filter)
			{
				auto& transf = ent.getComponent<core::transform>();
				auto& lightComp = ent.getComponent<light>();

				switch (lightComp.type)
				{
				case LightType::DIRECTIONAL:
					lightComp.dir_data.lightProjection = lightProjection;
					lightComp.dir_data.lightView = math::lookAt(math::vec3::zero, -transf.forward(), transf.up());
					lightComp.dir_data.direction = transf.forward();
					lightDataStruct.dir_data[0] = lightComp.dir_data;
					break;
				case LightType::POINT:
					if (m_lastIdx >= MAX_POINT_LIGHT_COUNT) return;
					lightComp.point_data.position = transf.position;
					lightComp.point_data.farPlane = far_plane;
					lightComp.point_data.shadowProjection = shadowProjection;
					buildShadowCube(lightComp.point_data.shadowTransforms, transf.position);
					if (lightComp.index < 0)
					{
						lightComp.index = m_lastIdx++;
						lightInfo.count++;
					}
					if (lightComp.index < MAX_POINT_LIGHT_COUNT)
						lightDataStruct.point_data[lightComp.index] = lightComp.point_data;
					break;
				case LightType::SPOT:
					//lightComp.data.position = transf.position;
					break;
				}
			}

			if (lightBuffer != nullptr)
			{
				lightBuffer->bufferData(&lightDataStruct, 1);
			}
		}

		virtual rsl::priority_type priority() const override { return LIGHT_PRIORITY; }

		void buildShadowCube(math::mat4* transfArray, math::vec3 lightPos)
		{
			transfArray[0] = math::lookAt(lightPos, lightPos + math::vec3::left, math::vec3::up);
			transfArray[1] = math::lookAt(lightPos, lightPos + math::vec3::right, math::vec3::up);
			transfArray[2] = math::lookAt(lightPos, lightPos + math::vec3::up, math::vec3::forward);
			transfArray[3] = math::lookAt(lightPos, lightPos + math::vec3::down, math::vec3::backward);
			transfArray[4] = math::lookAt(lightPos, lightPos + math::vec3::backward, math::vec3::up);
			transfArray[5] = math::lookAt(lightPos, lightPos + math::vec3::forward, math::vec3::up);
		}
	};

}