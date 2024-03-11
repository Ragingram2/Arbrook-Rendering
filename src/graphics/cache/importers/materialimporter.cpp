#include "graphics/cache/importers/materialimporter.hpp"

namespace fs = std::filesystem;
namespace ast = rythe::core::assets;

template<>
std::unordered_map<rsl::id_type, std::unique_ptr<ast::AssetImporter<rythe::rendering::material>>> ast::AssetCache<rythe::rendering::material>::m_importers;
template<>
std::unordered_map<rsl::id_type, std::unique_ptr<rythe::rendering::material>> ast::AssetCache<rythe::rendering::material>::m_assets;
template<>
std::unordered_map<rsl::id_type, std::string> ast::AssetCache<rythe::rendering::material>::m_names;

namespace rythe::rendering
{
	bool MaterialImporter::canLoad(fs::path filePath)
	{
		return m_importer.IsExtensionSupported(filePath.extension().string().c_str());
	}

	ast::asset_handle<material> MaterialImporter::load(rsl::id_type id, fs::path filePath, material* data, const ast::import_settings<material>& settings)
	{
		return { 0, nullptr };
	}

	void MaterialImporter::free(material&) {}

	void MaterialImporter::initMat(material* data, const aiMaterial* _material)
	{

	}
}