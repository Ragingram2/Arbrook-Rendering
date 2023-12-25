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

	struct light_data
	{
		math::vec4 direction = math::vec4(0.0f);
		math::vec4 position = math::vec4(0.0f);
		math::vec4 color = math::vec4(0.0f);
		float range = 0.0f;
		float intensity = 0.0f;
	};

	struct light
	{
		LightType type;
		rsl::uint index;
		light_data data;
	};

}