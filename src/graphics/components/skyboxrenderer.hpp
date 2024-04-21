#pragma once
#include "core/assets/assethandle.hpp"
#include "graphics/interface/definitions/texture.hpp"

namespace rythe::rendering
{
	struct skybox_renderer
	{
		bool enabled = true;
		texture_handle skyboxTex;
	};
}