#pragma once
#include "core/assets/assethandle.hpp"
#include "graphics/interface/definitions/shader.hpp"
#include "graphics/interface/definitions/texture.hpp"
#include "graphics/interface/definitions/definitions.hpp"
#include "graphics/data/vertex.hpp"
#include "graphics/data/mesh.hpp"

namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	struct sprite_renderer
	{
		ast::asset_handle<mesh> mesh;
		ast::asset_handle<texture> sprite;
		ast::asset_handle<shader> shaderHandle;
		inputlayout layout;

		//sprite_renderer() = default;
		//sprite_renderer(ast::asset_handle<texture> t_handle, ast::asset_handle<shader> s_handle) : sprite(t_handle), shaderHandle(s_handle) {}

	};
}