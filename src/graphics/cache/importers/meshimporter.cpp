#include "graphics/cache/importers/meshimporter.hpp"

namespace fs = std::filesystem;
namespace ast = rythe::core::assets;

template<>
std::unordered_map<rsl::id_type, std::unique_ptr<ast::AssetImporter<rythe::rendering::mesh>>> ast::AssetCache<rythe::rendering::mesh>::m_importers;
template<>
std::unordered_map<rsl::id_type, std::unique_ptr<rythe::rendering::mesh>> ast::AssetCache<rythe::rendering::mesh>::m_assets;
template<>
std::unordered_map<rsl::id_type, std::string> ast::AssetCache<rythe::rendering::mesh>::m_names;