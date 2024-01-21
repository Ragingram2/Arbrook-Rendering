#pragma once
#include "graphics/interface/iframebuffer.hpp"
#include "graphics/interface/config.hpp"
#include Framebuffer_HPP_PATH

namespace rythe::rendering
{
	typedef Iframebuffer<internal::framebuffer> framebuffer;
}