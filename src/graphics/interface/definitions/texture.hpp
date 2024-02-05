#pragma once
#include "graphics/interface/definitions/enumtypes.hpp"
#include "graphics/interface/itexture.hpp"
#include "graphics/interface/config.hpp"
#include Texture_HPP_PATH

namespace rythe::rendering
{
	typedef Itexture<internal::texture> texture;

#define DEPTH_STENCIL_SLOT TextureSlot::TEXTURE0
#define COLOR_SLOT TextureSlot::TEXTURE1
#define TEXTURE0_SLOT TextureSlot::TEXTURE2
#define TEXTURE1_SLOT TextureSlot::TEXTURE3
#define TEXTURE2_SLOT TextureSlot::TEXTURE4
#define TEXTURE3_SLOT TextureSlot::TEXTURE5


}

