#pragma once
#include <string>
#include <filesystem>

#include <rsl/math>

namespace rythe::rendering
{
	namespace fs = std::filesystem;
	struct texture_source
	{
		std::string name;
		fs::path filePath;
		unsigned char* data;
		math::ivec2 resolution;
		int channels;
	};
}