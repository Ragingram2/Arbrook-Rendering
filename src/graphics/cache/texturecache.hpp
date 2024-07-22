#pragma once
#include <unordered_map>
#include <rsl/primitives>
#include <rsl/logging>

#include "core/assets/assethandle.hpp"
#include "graphics/data/textureparameters.hpp"
#include "graphics/data/texturehandle.hpp"
#include "graphics/data/texturesource.hpp"
#include "graphics/interface/definitions/texture.hpp"


namespace fs = std::filesystem;
namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	namespace log = rsl::log;
	class TextureCache
	{
	private:
		static std::unordered_map<rsl::id_type, std::unique_ptr<texture>> m_textures;
		static std::unordered_map<rsl::id_type, std::string> m_names;
	public:
		static texture_handle createTexture(const std::string& name, TextureType targetType, ast::asset_handle<texture_source> textureData, math::ivec2 overrideResolution, texture_parameters params = default_texture_params);
		static texture_handle createTexture(const std::string& name, TextureType targetType, ast::asset_handle<texture_source> textureData, texture_parameters params = default_texture_params);
		static texture_handle createTexture2D(const std::string& name, ast::asset_handle<texture_source> textureData, math::ivec2 overrideResolution, texture_parameters params = default_texture_params);
		static texture_handle createTexture2D(const std::string& name, ast::asset_handle<texture_source> textureData, texture_parameters params = default_texture_params);
		static texture_handle createTexture2D(ast::asset_handle<texture_source> textureData, texture_parameters params = default_texture_params);
		static texture_handle createTexture2D(const std::string& name, texture_parameters params = default_texture_params);
		static texture_handle createCubemap(const std::string& name, ast::asset_handle<texture_source> textureData, math::ivec2 overrideResolution, texture_parameters params = default_texture_params);
		static texture_handle createCubemap(const std::string& name, ast::asset_handle<texture_source> textureData, texture_parameters params = default_texture_params);
		//static texture_handle createCubemap(const std::string& name, ast::asset_handle<texture_source>* textureDataArray, texture_parameters params = default_texture_params);

		static void resetTextures();

		static texture_handle getTexture(const std::string& name);
		static texture_handle getTexture(rsl::id_type nameHash);
		static void deleteTexture(const std::string& name);
		static void deleteTexture(rsl::id_type nameHash);
		static std::vector<texture_handle> getTextures();
		static std::vector<std::string> getTextureNames();
		static std::vector<const char*> getTextureNamesC();
	};
}