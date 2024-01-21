#include "graphics/cache/shadercache.hpp"

namespace rythe::rendering
{
	std::unordered_map<rsl::id_type, std::unique_ptr<shader>> ShaderCache::m_shaders;
	std::unordered_map<rsl::id_type, std::string> ShaderCache::m_names;

	shader_handle ShaderCache::createShader(const std::string& name, ast::asset_handle<shader_source> shaderSource)
	{
		rsl::id_type id = rsl::nameHash(name);
		if (m_shaders.count(id))
		{
			log::warn("This shader name \"{}\" already exists, returning existing handle", name);
			return { m_shaders[id].get() };
		}

		auto shad = m_shaders.emplace(id, std::make_unique<shader>()).first->second.get();
		shad->m_impl.initialize(name, shaderSource);
		m_names.emplace(id, name);
		return { shad };
	}

	void ShaderCache::compileShaders(std::vector<ast::asset_handle<shader_source>> assets)
	{
		for (auto& source : assets)
		{
			createShader(source->fileName, source);
		}
	}

	shader_handle ShaderCache::getShader(const std::string& name)
	{
		return getShader(rsl::nameHash(name));
	}

	shader_handle ShaderCache::getShader(rsl::id_type nameHash)
	{
		if (m_shaders.count(nameHash))
		{
			return { m_shaders[nameHash].get() };
		}
		log::error("Shader \"{}\" does not exist");
		return { nullptr };
	}

	void ShaderCache::deleteShader(const std::string& name)
	{
		deleteShader(rsl::nameHash(name));
	}

	void ShaderCache::deleteShader(rsl::id_type nameHash)
	{
		if (m_shaders.count(nameHash))
		{
			m_shaders[nameHash]->release();
			m_shaders.erase(nameHash);
			m_names.erase(nameHash);
		}
	}

	std::vector<shader_handle> ShaderCache::getShaders()
	{
		std::vector<shader_handle> handles;
		for (auto& [id, handle] : m_shaders)
		{
			handles.emplace_back(handle.get());
		}
		return handles;
	}

	std::vector<std::string> ShaderCache::getShaderNames()
	{
		std::vector<std::string> names;
		for (auto& [id, name] : m_names)
		{
			names.push_back(name);
		}
		return names;
	}

	std::vector<const char*> ShaderCache::getShaderNamesC()
	{
		std::vector<const char*> names;
		for (auto& [id, name] : m_names)
		{
			names.push_back(name.c_str());
		}
		return names;
	}

}