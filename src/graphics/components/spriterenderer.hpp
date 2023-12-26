#pragma once
#include "graphics/interface/definitions/definitions.hpp"
#include "graphics/data/meshhandle.hpp"
#include "graphics/data/shaderhandle.hpp"
#include "graphics/data/texturehandle.hpp"
#include "graphics/data/vertex.hpp"

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