#pragma once
#include <rsl/math>
#include <rsl/primitives>

namespace rythe::rendering
{
	enum struct LightType : rsl::uint
	{
		DIRECTIONAL = 0,
		POINT = 1,
		SPOT = 2
	};

	struct point_light_data
	{
		math::mat4 shadowProjection = math::mat4();
		math::mat4 shadowTransforms[6];
		math::vec4 position = math::vec4(0.0f);
		math::vec4 color = math::vec4(0.0f);
		float intensity = 0.0f;
		float range = 0.0f;
		float farPlane = 0.0f;
	};

	struct dir_light_data
	{
		math::mat4 lightProjection = math::mat4();
		math::mat4 lightView = math::mat4();
		math::vec4 direction = math::vec4(0.0f);
		math::vec4 color = math::vec4(0.0f);
		float intensity = 0.0f;
	};

	struct light
	{
		LightType type;
		int index = -1;
		point_light_data point_data;
		dir_light_data dir_data;
	};

}