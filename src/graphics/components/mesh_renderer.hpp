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
		bool enabled = true;
		bool dirty = true;
		bool instanced = false;
		bool castShadows = true;
		inputlayout layout;
		std::unordered_map<rsl::id_type, material> materials;
		material mainMaterial;
		model model;
	};
}