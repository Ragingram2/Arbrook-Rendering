#pragma once
#include <string>
#include <rsl/math>

namespace rythe::rendering
{
	struct texture_source
	{
		std::string name;
		std::string filePath;
		unsigned char* data;
		math::ivec2 resolution;
		int channels;
	};
}