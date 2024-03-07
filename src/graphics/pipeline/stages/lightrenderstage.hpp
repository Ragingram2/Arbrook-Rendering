#pragma once
#include <rsl/primitives>
#include <rsl/utilities>
#include <rsl/math>

#include "core/events/defaults/component_event.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/interface/definitions/definitions.hpp"

namespace rythe::rendering
{
	struct point_light_buffer
	{
		point_light_data point_data[MAX_POINT_LIGHT_COUNT];
	};

	struct dir_light_buffer
	{
		dir_light_data dir_data[1];
	};

	struct light_render_stage : public graphics_stage<light_render_stage, light>
	{
		rsl::uint m_lastIdx = 0;
		buffer_handle pointLightBuffer;
		buffer_handle directionalLightBuffer;
		point_light_buffer pointLightData;
		dir_light_buffer dirLightData;

		math::mat4 lightProjection;
		math::mat4 shadowProjection;
		std::vector<math::mat4> shadowTransforms;

		float far_plane = 50.0f;

		virtual void setup(core::transform camTransf, camera& cam) override
		{
			pointLightBuffer = BufferCache::createConstantBuffer<point_light_buffer>("PointLightBuffer", SV_PT_LIGHTS, UsageType::STATICDRAW);
			directionalLightBuffer = BufferCache::createConstantBuffer<dir_light_buffer>("DirectionalLightBuffer", SV_DIR_LIGHTS, UsageType::STATICDRAW);

			lightProjection = math::orthographic(-30.0f, 30.0f, -30.0f, 30.0f, -10.0f, 40.0f);
			shadowProjection = math::perspective(90.0f, Shadow_Width / Shadow_Height, 1.0f, far_plane);
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{

			ZoneScopedN("[Renderer] [Light Stage] Render");
			for (auto& ent : m_filter)
			{
				auto& transf = ent.getComponent<core::transform>();
				auto& lightComp = ent.getComponent<light>();
				auto lightView = math::lookAt(transf.forward(), math::vec3::zero, transf.up());

				switch (lightComp.type)
				{
				case LightType::DIRECTIONAL:
					lightComp.dir_data.lightProjection = lightProjection;
					lightComp.dir_data.lightView = lightView;
					lightComp.dir_data.direction = transf.forward();
					dirLightData.dir_data[0] = lightComp.dir_data;
					break;
				case LightType::POINT:
					if (m_lastIdx >= MAX_POINT_LIGHT_COUNT) return;
					lightComp.point_data.shadowProjection = shadowProjection;
					buildShadowCube(lightComp.point_data.shadowTransforms, transf.position);
					lightComp.point_data.position = transf.position;
					lightComp.point_data.farPlane = far_plane;
					if (lightComp.index < 0)
					{
						lightComp.index = m_lastIdx++;
						lightInfo.count += 1;
					}
					if (lightComp.index < MAX_POINT_LIGHT_COUNT)
						pointLightData.point_data[lightComp.index] = lightComp.point_data;
					break;
				case LightType::SPOT:
					//lightComp.data.position = transf.position;
					break;
				}
			}

			if (pointLightBuffer != nullptr)
				pointLightBuffer->bufferData(&pointLightData, 1);

			if (directionalLightBuffer != nullptr)
				directionalLightBuffer->bufferData(&dirLightData, 1);
		}

		virtual rsl::priority_type priority() const override { return LIGHT_PRIORITY; }

		void buildShadowCube(math::mat4* transfArray, math::vec3 lightPos)
		{
			transfArray[0] = math::lookAt( lightPos, lightPos + math::vec3::left, math::vec3::up);
			transfArray[1] = math::lookAt( lightPos, lightPos + math::vec3::right, math::vec3::up);
#ifdef RenderingAPI_OGL
			transfArray[2] = math::lookAt(lightPos, lightPos + math::vec3::down, math::vec3::backward);
			transfArray[3] = math::lookAt(lightPos, lightPos + math::vec3::up, math::vec3::forward);
#else
			transfArray[2] = math::lookAt(lightPos, lightPos + math::vec3::up, math::vec3::forward);
			transfArray[3] = math::lookAt(lightPos, lightPos + math::vec3::down, math::vec3::backward);
#endif
			transfArray[4] = math::lookAt(lightPos, lightPos + math::vec3::backward, math::vec3::up);
			transfArray[5] = math::lookAt( lightPos, lightPos + math::vec3::forward, math::vec3::up);
		}
	};

}