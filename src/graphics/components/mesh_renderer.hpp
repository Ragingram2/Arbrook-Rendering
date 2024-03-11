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
		ast::asset_handle<material> material {0, nullptr};
		ast::asset_handle<model> model {0,nullptr};

		//mesh_renderer() = default;
		//mesh_renderer(bool _dirty = true, bool _instanced = false, bool _castShadows = true, inputlayout _layout = inputlayout{}, ast::asset_handle<struct material> _mat = nullptr, ast::asset_handle<struct model> _model) : dirty(_dirty), instanced(_instanced), castShadows(_castShadows), layout(_layout), material(_mat), model(_model) {}
	};
}