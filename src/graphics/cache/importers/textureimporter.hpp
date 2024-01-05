#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <rsl/logging>

#include "core/assets/assets.hpp"
#include "graphics/data/textureparameters.hpp"
#include "graphics/interface/definitions/texture.hpp"


namespace fs = std::filesystem;
namespace ast = rythe::core::assets;

namespace rythe::rendering
{
	namespace log = rsl::log;
	class TextureImporter : public ast::AssetImporter<texture>
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

		virtual ast::asset_handle<texture> load(rsl::id_type id, fs::path filePath, texture* data, const ast::import_settings<texture>& settings) override
		{
			auto name = filePath.stem().string();
			data->m_impl.initialize(static_cast<internal::TargetType>(settings.targetType), settings);
			data->m_impl.name = name;
			data->loadData(filePath.string());

			return { id, data };
		}

		virtual void free(texture& asset) override
		{

		}
	};
}