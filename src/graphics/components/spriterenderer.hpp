#pragma once
#include "core/assets/assethandle.hpp"
#include "graphics/data/material.hpp"
#include "graphics/interface/definitions/texture.hpp"
#include "graphics/interface/definitions/definitions.hpp"
#include "graphics/data/mesh.hpp"

namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	struct sprite_renderer
	{
		texture_handle sprite;
		ast::asset_handle<mesh> mesh;
		ast::asset_handle<material> material;
		inputlayout layout;
	};
}