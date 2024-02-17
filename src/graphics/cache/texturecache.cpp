#include "graphics/cache/texturecache.hpp"

namespace rythe::rendering
{

	std::unordered_map<rsl::id_type, std::unique_ptr<texture>> TextureCache::m_textures;
	std::unordered_map<rsl::id_type, std::string> TextureCache::m_names;

	texture_handle TextureCache::createTexture(const std::string& name, TargetType targetType, ast::asset_handle<texture_source> textureData, math::ivec2 overrideResolution, texture_parameters params)
	{
		rsl::id_type id = rsl::nameHash(name);
		if (m_textures.count(id))
		{
			log::warn("Texture \"{}\" already exists, returning existing handle", name);
			return { m_textures[id].get() };
		}

		auto& tex = m_textures.emplace(id, std::make_unique<texture>()).first->second;
		tex->m_impl.name = name;
		tex->m_impl.channels = textureData != nullptr ? textureData->channels : 4;
		tex->m_impl.resolution = overrideResolution;
		tex->m_impl.initialize(static_cast<internal::TargetType>(targetType), params);
		tex->loadData(textureData != nullptr ? textureData->data : nullptr);
		m_names.emplace(id, name);
		return { tex.get() };
	}

	texture_handle TextureCache::createTexture(const std::string& name, TargetType targetType, ast::asset_handle<texture_source> textureData, texture_parameters params)
	{
		return createTexture(name, targetType, textureData, textureData->resolution, params);
	}

	texture_handle TextureCache::createTexture2D(const std::string& name, ast::asset_handle<texture_source> textureData, math::ivec2 overrideResolution, texture_parameters params)
	{
		rsl::id_type id = rsl::nameHash(name);
		if (m_textures.count(id))
		{
			log::warn("Texture \"{}\" already exists, returning existing handle", name);
			return { m_textures[id].get() };
		}

		auto& tex = m_textures.emplace(id, std::make_unique<texture>()).first->second;
		tex->m_impl.name = name;
		tex->m_impl.channels = textureData != nullptr ? textureData->channels : 4;
		tex->m_impl.resolution = overrideResolution;
		tex->m_impl.initialize(internal::TargetType::TEXTURE2D, params);
		tex->loadData(textureData != nullptr ? textureData->data : nullptr);
		m_names.emplace(id, name);
		return { tex.get() };
	}

	texture_handle TextureCache::createTexture2D(const std::string& name, ast::asset_handle<texture_source> textureData, texture_parameters params)
	{
		return createTexture2D(name, textureData, textureData->resolution, params);
	}

	texture_handle TextureCache::createCubemap(const std::string& name, ast::asset_handle<texture_source> textureData, math::ivec2 overrideResolution, texture_parameters params)
	{
		rsl::id_type id = rsl::nameHash(name);
		if (m_textures.count(id))
		{
			log::warn("Texture \"{}\" already exists, returning existing handle", name);
			return { m_textures[id].get() };
		}

		auto& tex = m_textures.emplace(id, std::make_unique<texture>()).first->second;
		tex->m_impl.name = name;
		tex->m_impl.channels = textureData != nullptr ? textureData->channels : 4;
		tex->m_impl.resolution = overrideResolution;
		tex->m_impl.initialize(internal::TargetType::CUBEMAP, params);
		tex->loadData(textureData != nullptr ? textureData->data : nullptr);
		m_names.emplace(id, name);
		return { tex.get() };
	}

	texture_handle TextureCache::createCubemap(const std::string& name, ast::asset_handle<texture_source> textureData, texture_parameters params)
	{
		return createCubemap(name, textureData, textureData->resolution, params);
	}

	texture_handle TextureCache::getTexture(const std::string& name)
	{
		return getTexture(rsl::nameHash(name));
	}

	texture_handle TextureCache::getTexture(rsl::id_type nameHash)
	{
		if (m_textures.count(nameHash))
		{
			return { m_textures[nameHash].get() };
		}

		log::error("The texture you tried to retrieve does not exist");
		return { nullptr };
	}

	void TextureCache::deleteTexture(const std::string& name)
	{
		deleteTexture(rsl::nameHash(name));
	}

	void TextureCache::deleteTexture(rsl::id_type nameHash)
	{
		if (m_textures.count(nameHash))
		{
			m_textures.erase(nameHash);
			m_names.erase(nameHash);
		}
	}

	std::vector<texture_handle> TextureCache::getTextures()
	{
		std::vector<texture_handle> handles;
		for (auto& [id, handle] : m_textures)
		{
			handles.emplace_back(handle.get());
		}
		return handles;
	}

	std::vector<std::string> TextureCache::getTextureNames()
	{
		std::vector<std::string> names;
		for (auto& [id, name] : m_names)
		{
			names.push_back(name);
		}
		return names;
	}

	std::vector<const char*> TextureCache::getTextureNamesC()
	{
		std::vector<const char*> names;
		for (auto& [id, name] : m_names)
		{
			names.push_back(name.c_str());
		}
		return names;
	}

}