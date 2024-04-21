#include "graphics/pipeline/stages/lightrenderstage.hpp"

namespace rythe::rendering
{
	void light_render_stage::setup(core::transform camTransf, camera& cam)
	{
		pointLightBuffer = BufferCache::createConstantBuffer<point_light_buffer>("PointLightBuffer", SV_PT_LIGHTS, UsageType::STATICDRAW);
		directionalLightBuffer = BufferCache::createConstantBuffer<dir_light_buffer>("DirectionalLightBuffer", SV_DIR_LIGHTS, UsageType::STATICDRAW);

		lightProjection = math::orthographic(-30.0f, 30.0f, -30.0f, 30.0f, -10.0f, 40.0f);
		shadowProjection = math::perspective(90.0f, Shadow_Width / Shadow_Height, 1.0f, far_plane);
	}

	void light_render_stage::render(core::transform camTransf, camera& cam)
	{
		ZoneScopedN("[Renderer] [Light Stage] Render");
		int lightCount = 0;
		for (auto& ent : m_filter)
		{
			auto& transf = ent.getComponent<core::transform>();
			auto& lightComp = ent.getComponent<light>();
			if (!ent->enabled || !lightComp.enabled)
			{
				if (lightComp.type == LightType::DIRECTIONAL)
					dirLightData.dir_data[0].intensity = 0.0f;
				else
				{
					if (lightComp.index > -1)
						pointLightData.point_data[lightComp.index].intensity = 0.0f;
				}

				lightComp.index = -1;
				continue;
			}
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
				lightCount++;
				lightComp.point_data.shadowProjection = shadowProjection;
				buildShadowCube(lightComp.point_data.shadowTransforms, transf.position);
				lightComp.point_data.position = transf.position;
				lightComp.point_data.farPlane = far_plane;
				if (lightComp.index < 0)
				{
					lightComp.index = m_lastIdx++;
				}
				if (lightComp.index < MAX_POINT_LIGHT_COUNT)
					pointLightData.point_data[lightComp.index] = lightComp.point_data;
				break;
			case LightType::SPOT:
				//lightComp.data.position = transf.position;
				break;
			}
		}

		lightInfo.count = lightCount;
		m_lastIdx = lightInfo.count;

		if (pointLightBuffer)
			pointLightBuffer->bufferData(&pointLightData, 1);

		if (directionalLightBuffer)
			directionalLightBuffer->bufferData(&dirLightData, 1);
	}

	rsl::priority_type light_render_stage::priority() const { return LIGHT_PRIORITY; }

	void light_render_stage::buildShadowCube(math::mat4* transfArray, math::vec3 lightPos)
	{
		transfArray[0] = math::lookAt(lightPos, lightPos + math::vec3::left, math::vec3::up);
		transfArray[1] = math::lookAt(lightPos, lightPos + math::vec3::right, math::vec3::up);
#ifdef RenderingAPI_OGL
		transfArray[2] = math::lookAt(lightPos, lightPos + math::vec3::down, math::vec3::backward);
		transfArray[3] = math::lookAt(lightPos, lightPos + math::vec3::up, math::vec3::forward);
#else
		transfArray[2] = math::lookAt(lightPos, lightPos + math::vec3::up, math::vec3::forward);
		transfArray[3] = math::lookAt(lightPos, lightPos + math::vec3::down, math::vec3::backward);
#endif
		transfArray[4] = math::lookAt(lightPos, lightPos + math::vec3::backward, math::vec3::up);
		transfArray[5] = math::lookAt(lightPos, lightPos + math::vec3::forward, math::vec3::up);
	}
}