#pragma once

#include "core/components/property.hpp"
#include "graphics/data/materialhandle.hpp"
#include "graphics/data/modelhandle.hpp"
#include "graphics/data/meshhandle.hpp"
#include "graphics/cache/modelcache.hpp"

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