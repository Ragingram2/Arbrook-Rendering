#include "graphics/cache/importers/textureimporter.hpp"
#include "core/assets/assetcache.hpp"
#include "core/assets/assetimporter.hpp"
#include <stb/stb_image.h>


template<>
std::unordered_map<rsl::id_type, std::unique_ptr<rythe::core::assets::AssetImporter<rythe::rendering::texture_source>>> rythe::core::assets::AssetCache<rythe::rendering::texture_source>::m_importers;
template<>
std::unordered_map<rsl::id_type, std::unique_ptr<rythe::rendering::texture_source>> rythe::core::assets::AssetCache<rythe::rendering::texture_source>::m_assets;
template<>
std::unordered_map<rsl::id_type, std::string> rythe::core::assets::AssetCache<rythe::rendering::texture_source>::m_names;

namespace rythe::rendering
{
	bool TextureImporter::canLoad(fs::path filePath)
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

	ast::asset_handle<texture_source> TextureImporter::load(rsl::id_type id, fs::path filePath, texture_source* data, const ast::import_settings<texture_source>& settings)
	{
		auto name = filePath.stem().string();
#ifndef RenderingAPI_DX11
		stbi_set_flip_vertically_on_load(settings.flipVertical);
#endif
		data->name = filePath.stem().string();
		data->filePath = filePath.string();
		data->data = std::move(stbi_load(filePath.string().c_str(), &data->resolution.x, &data->resolution.y, &data->channels, 0));
		if (!data)
			log::error("Image \"{}\" failed to load", filePath.string());
		return { id, data };
	}

	ast::asset_handle<texture_source> TextureImporter::loadFromMemory(rsl::id_type id, texture_source* data, const ast::import_settings<texture_source>& settings)
	{
		return { id, data };
	}

	void TextureImporter::write(fs::path filePath, texture_source* data)
	{
		//std::ofstream o(filePath);
		//nlohmann::json j;
		//ns::to_json(j, *data);
		//o << std::setw(4) << j << std::endl;
	}

	void TextureImporter::free(texture_source& asset) { }
}