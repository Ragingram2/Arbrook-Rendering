#pragma once
#include "core/assets/assethandle.hpp"
#include "graphics/interface/definitions/texture.hpp"

namespace rythe::rendering
{
	struct skybox_renderer
	{
		ast::asset_handle<texture> skyboxTex;
	};
}