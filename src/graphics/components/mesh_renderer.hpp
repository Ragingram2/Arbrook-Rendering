#pragma once
#include "core/assets/assethandle.hpp"
#include "core/components/property.hpp"
#include "graphics/data/material.hpp"
#include "graphics/data/model.hpp"

namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	struct mesh_renderer
	{
		bool dirty = true;
		bool instanced = false;
		ast::asset_handle<material> material;
		ast::asset_handle<model> model;
	};
}