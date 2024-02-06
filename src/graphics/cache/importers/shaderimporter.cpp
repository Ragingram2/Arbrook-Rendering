#include "graphics/cache/importers/shaderimporter.hpp"

template<>
std::unordered_map<rsl::id_type, std::unique_ptr<rythe::core::assets::AssetImporter<rythe::rendering::shader_source>>> rythe::core::assets::AssetCache<rythe::rendering::shader_source>::m_importers;
template<>
std::unordered_map<rsl::id_type, std::unique_ptr<rythe::rendering::shader_source>> rythe::core::assets::AssetCache<rythe::rendering::shader_source>::m_assets;
template<>
std::unordered_map<rsl::id_type, std::string> rythe::core::assets::AssetCache<rythe::rendering::shader_source>::m_names;