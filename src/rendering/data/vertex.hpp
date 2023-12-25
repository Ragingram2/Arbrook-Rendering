#pragma once
#include "core/math/math.hpp"

namespace rythe::rendering
{
	struct vertex
	{
		math::vec3 m_position;
		math::vec2 m_uv;
		vertex() = default;
		vertex(math::vec3 position, math::vec2 uv) : m_position(position), m_uv(uv) {}
		~vertex() = default;
	};

	struct vtx_constant
	{
		math::vec3 position;
		float time;
	};

}