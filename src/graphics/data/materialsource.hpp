#pragma once
#include <string>
#include <sstream>
#include <utility>
#include <filesystem>

#include <rsl/primitives>

#include "graphics/data/material.hpp"

namespace rythe::rendering
{
	namespace fs = std::filesystem;
	struct material_source
	{
		std::string name;
		fs::path filePath;

		std::string shaderName;
		rsl::uint shaderId = 0;

		std::vector<std::string> textureFilepaths;
	};
}