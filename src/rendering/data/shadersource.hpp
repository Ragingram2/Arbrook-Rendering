#pragma once
#include <string>
#include <sstream>
#include <utility>

#include "rendering/interface/definitions/enumtypes.hpp"

namespace rythe::rendering
{
	struct shader_source
	{
		std::string fileName;
		std::pair<ShaderType, std::string> sources[5];

		shader_source() = default;
		shader_source(std::string _fileName, std::stringstream _ss[])
		{
			fileName = _fileName;
			sources[0] = std::make_pair<ShaderType,std::string>(ShaderType::VERTEX,_ss[0].str());
			sources[1] = std::make_pair<ShaderType, std::string>(ShaderType::FRAGMENT,_ss[1].str());
			sources[2] = std::make_pair<ShaderType, std::string>(ShaderType::GEOMETRY,_ss[2].str());
			sources[3] = std::make_pair<ShaderType, std::string>(ShaderType::HULL,_ss[3].str());
			sources[4] = std::make_pair<ShaderType, std::string>(ShaderType::COMPUTE,_ss[4].str());
		}
	};
}