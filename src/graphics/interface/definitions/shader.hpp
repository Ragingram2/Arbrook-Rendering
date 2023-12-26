#pragma once
#include "graphics/interface/ishader.hpp"
#include "graphics/interface/config.hpp"
#include Shader_HPP_PATH

namespace rythe::rendering
{
	typedef Ishader<internal::shader> shader;
}
