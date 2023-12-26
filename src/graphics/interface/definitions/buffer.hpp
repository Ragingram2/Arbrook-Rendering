#pragma once
#include "graphics/interface/ibuffer.hpp"
#include "graphics/interface/config.hpp"
#include Buffer_HPP_PATH

namespace rythe::rendering
{
	typedef Ibuffer<internal::buffer> buffer;
}