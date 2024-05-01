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

		float far_plane = 500.0f;

		virtual void setup(core::transform camTransf, camera& cam) override;
		virtual void render(core::transform camTransf, camera& cam) override;
		virtual rsl::priority_type priority() const override;
		void buildShadowCube(math::mat4* transfArray, math::vec3 lightPos);
	};

}