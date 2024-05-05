#pragma once
#include <unordered_map>

#include <rsl/primitives>
#include <rfl.hpp>
#include <rfl/json.hpp>

#include "core/assets/assethandle.hpp"
#include "core/components/property.hpp"
#include "graphics/data/material.hpp"
#include "graphics/data/model.hpp"



namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	struct mesh_rendererImpl
	{
		bool instanced = false;
		bool castShadows = true;
		materialImpl mainMaterial;
		modelImpl model;
	};

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

		mesh_rendererImpl reflectedType;

		mesh_rendererImpl& reflection()
		{
			reflectedType.castShadows = castShadows;
			reflectedType.instanced = instanced;
			reflectedType.mainMaterial.mat = &mainMaterial;
			reflectedType.model.model = &model;
			return reflectedType;
		}
	};
}