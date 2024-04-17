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
		bool castShadows = true;
		inputlayout layout;
		std::unordered_map<rsl::id_type, ast::asset_handle<material>> materials;
		ast::asset_handle<material> mainMaterial {0, nullptr};
		ast::asset_handle<model> model {0,nullptr};
	};
}