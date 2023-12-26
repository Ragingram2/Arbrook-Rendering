#pragma once
#include "graphics/interface/irenderinterface.hpp"
#include "graphics/interface/config.hpp"
#include RenderInterface_HPP_PATH

namespace rythe::rendering
{
	typedef IRenderInterface<internal::RenderInterface> RenderInterface;
}