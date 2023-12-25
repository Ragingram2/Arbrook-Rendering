#pragma once
#include "rendering/interface/ibuffer.hpp"
#include "rendering/interface/config.hpp"
#include Buffer_HPP_PATH

namespace rythe::rendering
{
	typedef Ibuffer<internal::buffer> buffer;
}