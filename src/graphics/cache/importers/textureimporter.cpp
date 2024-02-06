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