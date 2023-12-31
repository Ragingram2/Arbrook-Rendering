#include "graphics/cache/materialcache.hpp"
#include "graphics/cache/importers/shaderimporter.hpp"
#include "graphics/cache/importers/textureimporter.hpp"

namespace rythe::rendering
{
	std::unordered_map<rsl::id_type, std::unique_ptr<material>> MaterialCache::m_materials;
	std::unordered_map<rsl::id_type, std::string> MaterialCache::m_names;

	ast::asset_handle<material> MaterialCache::loadMaterial(const std::string& name, ast::asset_handle<shader> shader)
	{
		rsl::id_type id = rsl::nameHash(name);
		if (m_materials.contains(id))
		{
			log::warn("Material {} already exists, ignoring creation request and returning existing material", name);
			return { id, m_materials[id].get() };
		}

		std::unique_ptr<material> mat = std::make_unique<material>();
		mat->shader = shader;
		mat->name = name;
		m_materials.emplace(id, std::move(mat));
		return { id,  m_materials[id].get() };
	}
	ast::asset_handle<material> MaterialCache::loadMaterial(const std::string& name, const std::string& shaderName)
	{
		return loadMaterial(name, rsl::nameHash(shaderName));
	}
	ast::asset_handle<material> MaterialCache::loadMaterial(const std::string& name, rsl::id_type shaderId)
	{
		return loadMaterial(name, ast::AssetCache<shader>::getAsset(shaderId));
	}
	ast::asset_handle<material> MaterialCache::loadMaterialFromFile(const std::string& name, fs::path shaderPath)
	{
		return loadMaterial(name, ast::AssetCache<shader>::createAsset(name, shaderPath, default_shader_params, false));
	}
	ast::asset_handle<material> MaterialCache::getMaterial(const std::string& name)
	{
		rsl::id_type id = rsl::nameHash(name);
		if (m_materials.contains(id))
		{
			return { id, m_materials[id].get() };
		}
		return { 0, nullptr };
	}
	void MaterialCache::deleteMaterial(const std::string& name)
	{
		deleteMaterial(rsl::nameHash(name));
	}
	void MaterialCache::deleteMaterial(rsl::id_type nameHash)
	{
		if (m_materials.contains(nameHash))
		{
			m_materials[nameHash]->shader->clearBuffers();
			m_materials.erase(nameHash);
			m_names.erase(nameHash);
		}
	}
	std::vector<ast::asset_handle<material>> MaterialCache::getMaterials()
	{
		std::vector<ast::asset_handle<material>> handles;
		for (auto& [id, handle] : m_materials)
		{
			handles.emplace_back(id, handle.get());
		}
		return handles;
	}
	std::vector<std::string> MaterialCache::getMaterialNames()
	{
		std::vector<std::string> names;
		for (auto& [id, handle] : m_materials)
		{
			names.push_back(handle->name);
		}
		return names;

	}
	std::vector<const char*> MaterialCache::getMaterialNamesC()
	{
		std::vector<const char*> names;
		for (auto& [id, handle] : m_materials)
		{
			names.push_back(handle->name.c_str());
		}
		return names;
	}
}