#pragma once
#include "rendering/interface/definitions/definitions.hpp"
#include "rendering/data/meshhandle.hpp"
#include "rendering/data/shaderhandle.hpp"
#include "rendering/data/texturehandle.hpp"
#include "rendering/data/vertex.hpp"

namespace rythe::rendering
{
	struct sprite_renderer
	{
		mesh_handle mesh;
		texture_handle texture;
		shader_handle shader;
		inputlayout layout;

		sprite_renderer() = default;
		sprite_renderer(texture_handle t_handle, shader_handle s_handle) : texture(t_handle), shader(s_handle) {}

	};
}