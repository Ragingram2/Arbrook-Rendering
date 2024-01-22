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
		virtual bool canLoad(fs::path filePath) override
		{
			if (!filePath.has_extension())
			{
				log::warn("File path \"{}\" does not have a file extension", filePath.string());
				return false;
			}

			for (auto format : supportedFormats)
			{
				if (filePath.extension() == format)
					return true;
			}
			return false;
		}

		virtual ast::asset_handle<texture_source> load(rsl::id_type id, fs::path filePath, texture_source* data, const ast::import_settings<texture_source>& settings) override
		{
			auto name = filePath.stem().string();
			stbi_set_flip_vertically_on_load(settings.flipVertical);
			data->filePath = filePath.string();
			data->data = std::move(stbi_load(filePath.string().c_str(), &data->resolution.x, &data->resolution.y, &data->channels, 0));
			if (!data)
				log::error("Image \"{}\" failed to load", filePath.string());
			return { id, data };
		}

		virtual void free(texture_source& asset) override
		{

		}
	};
}