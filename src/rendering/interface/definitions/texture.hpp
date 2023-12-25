#pragma once
#include "rendering/interface/itexture.hpp"
#include "rendering/interface/config.hpp"
#include Texture_HPP_PATH

namespace rythe::rendering
{
	typedef Itexture<internal::texture> texture;
}