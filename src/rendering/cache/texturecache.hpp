#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "core/core.hpp"
#include "rendering/data/texturehandle.hpp"
#include "rendering/interface/definitions/texture.hpp"

namespace fs = std::filesystem;
namespace rythe::rendering
{
	class TextureCache
	{
	private:
		static std::unordered_map<std::string, std::unique_ptr<texture>> m_textures;
	public:
		static texture_handle createTexture2D(const std::string& name, const std::string& filepath, texture_parameters params = default_params);
		static texture_handle getTexture2D(const std::string& name);
		static void loadTextures(const std::string& directory);
	};
}