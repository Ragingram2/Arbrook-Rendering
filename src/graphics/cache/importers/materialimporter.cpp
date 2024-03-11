#include "graphics/cache/importers/materialimporter.hpp"

namespace fs = std::filesystem;
namespace ast = rythe::core::assets;

template<>
std::unordered_map<rsl::id_type, std::unique_ptr<ast::AssetImporter<rythe::rendering::material_source>>> ast::AssetCache<rythe::rendering::material_source>::m_importers;
template<>
std::unordered_map<rsl::id_type, std::unique_ptr<rythe::rendering::material_source>> ast::AssetCache<rythe::rendering::material_source>::m_assets;
template<>
std::unordered_map<rsl::id_type, std::string> ast::AssetCache<rythe::rendering::material_source>::m_names;

namespace rythe::rendering
{
	bool MaterialImporter::canLoad(fs::path filePath)
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

	ast::asset_handle<material_source> MaterialImporter::load(rsl::id_type id, fs::path filePath, material_source* data, const ast::import_settings<material_source>& settings)
	{
		std::ifstream f(filePath);

		nlohmann::json jsonData = nlohmann::json::parse(f);
		data->filePath = filePath;
		data->name = filePath.stem().string();
		data->shaderId = jsonData["shaderID"];
		data->shaderName = jsonData["shaderName"];
		data->textureFilepaths = jsonData["textureFilepaths"].get<std::vector<std::string>>();

		return { id, data };
	}

	void MaterialImporter::free(material_source&) {}

	//void MaterialImporter::initMat(material_source* data, const aiMaterial* _material)
	//{

	//}
}