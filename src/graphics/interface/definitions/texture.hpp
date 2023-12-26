#pragma once
#include "graphics/interface/itexture.hpp"
#include "graphics/interface/config.hpp"
#include Texture_HPP_PATH

namespace rythe::rendering
{
	typedef Itexture<internal::texture> texture;
}