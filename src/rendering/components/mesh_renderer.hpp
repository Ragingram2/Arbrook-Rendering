#pragma once

#include "core/components/property.hpp"
#include "rendering/data/materialhandle.hpp"
#include "rendering/data/modelhandle.hpp"
#include "rendering/data/meshhandle.hpp"
#include "rendering/cache/modelcache.hpp"

namespace rythe::rendering
{
	struct mesh_renderer
	{
		bool dirty = true;
		bool instanced = false;
		material_handle material;
		model_handle model;
	};
}