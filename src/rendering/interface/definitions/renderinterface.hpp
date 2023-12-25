#pragma once
#include "rendering/interface/irenderinterface.hpp"
#include "rendering/interface/config.hpp"
#include RenderInterface_HPP_PATH

namespace rythe::rendering
{
	typedef IRenderInterface<internal::RenderInterface> RenderInterface;
}