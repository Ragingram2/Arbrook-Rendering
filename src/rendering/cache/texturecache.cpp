#include "rendering/cache/texturecache.hpp"
#include <stb/stb_image.h>

namespace rythe::rendering
{
	std::unordered_map<std::string, std::unique_ptr<texture>> TextureCache::m_textures;

	texture_handle TextureCache::createTexture2D(const std::string& name, const std::string& filepath, texture_parameters params)
	{
		if (m_textures.contains(name))
		{
			log::warn("Texture {} already exists, ignoring new texture, and returning existing one", name);
			return m_textures[name].get();
		}
		auto tex = m_textures.emplace(name, std::make_unique<texture>()).first->second.get();
		tex->m_impl.name = name;
		tex->m_impl.initialize(internal::TargetType::TEXTURE2D, params);
		tex->loadData(filepath);

		return { tex };
	}

	texture_handle TextureCache::getTexture2D(const std::string& name)
	{
		if (m_textures.contains(name))
		{
			return m_textures[name].get();
		}
		log::warn("Texture {} does not exist", name);
		return texture_handle{ nullptr };
	}

	void TextureCache::loadTextures(const std::string& directory)
	{
		for (auto& p : fs::directory_iterator(directory))
		{
			if (!p.path().has_extension()) continue;
			if (p.path().extension() != ".png" && p.path().extension() != ".jpg") continue;

			auto fileName = p.path().stem().string();
			auto path = p.path().string();
			log::debug("Loading texture {} at \"{}\"", fileName, path);
			createTexture2D(fileName, path);
		}
	}
}