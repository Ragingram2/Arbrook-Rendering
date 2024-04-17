#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <rsl/logging>

#include <stb/stb_image.h>
#define STB_IMAGE_IMPLEMENTATION

#include "core/assets/assets.hpp"
#include "graphics/data/textureparameters.hpp"
#include "graphics/data/texturesource.hpp"

namespace fs = std::filesystem;
namespace ast = rythe::core::assets;

namespace rythe::core::assets
{
	template<>
	struct import_settings<rendering::texture_source>
	{
		bool flipVertical = false;
	};
}
namespace rythe::rendering
{
	using texture_import_parameters = core::assets::import_settings<rendering::texture_source>;

	constexpr core::assets::import_settings<rendering::texture_source> default_texture_import_params{
		.flipVertical = true
	};

	namespace log = rsl::log;
	class TextureImporter : public ast::AssetImporter<texture_source>
	{
	private:
		static constexpr const char* supportedFormats[] = { ".png", ".jpg" };
	public:
		virtual bool canLoad(fs::path filePath) override;
		virtual ast::asset_handle<texture_source> load(rsl::id_type id, fs::path filePath, texture_source* data, const ast::import_settings<texture_source>& settings) override;
		virtual ast::asset_handle<texture_source> loadFromMemory(rsl::id_type id, texture_source* data, const ast::import_settings<texture_source>& settings) override;
		virtual void write(fs::path filePath, texture_source* data) override;
		virtual void free(texture_source& asset) override;
	};
}