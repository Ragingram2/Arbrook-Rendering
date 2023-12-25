#pragma once
#include "rendering/interface/ishader.hpp"
#include "rendering/interface/config.hpp"
#include Shader_HPP_PATH

namespace rythe::rendering
{
	typedef Ishader<internal::shader> shader;
}
