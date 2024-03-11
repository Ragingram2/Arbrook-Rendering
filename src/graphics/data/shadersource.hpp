#pragma once
#include <string>
#include <sstream>
#include <utility>
#include <filesystem>

#include "graphics/interface/definitions/enumtypes.hpp"

namespace rythe::rendering
{
	namespace fs = std::filesystem;
	struct shader_source
	{
		std::string name;
		fs::path filePath;
		std::pair<ShaderType, std::string> sources[5];

		shader_source() = default;
		shader_source(std::string _name, fs::path _filePath, std::stringstream _ss[])
		{
			name = _name;
			filePath = _filePath;
			sources[0] = std::make_pair<ShaderType,std::string>(ShaderType::VERTEX,_ss[0].str());
			sources[1] = std::make_pair<ShaderType, std::string>(ShaderType::FRAGMENT,_ss[1].str());
			sources[2] = std::make_pair<ShaderType, std::string>(ShaderType::GEOMETRY,_ss[2].str());
			sources[3] = std::make_pair<ShaderType, std::string>(ShaderType::HULL,_ss[3].str());
			sources[4] = std::make_pair<ShaderType, std::string>(ShaderType::COMPUTE,_ss[4].str());
		}
	};
}