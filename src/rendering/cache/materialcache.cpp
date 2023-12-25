#include "rendering/cache/materialcache.hpp"
#include "rendering/cache/shadercache.hpp"
#include "rendering/cache/texturecache.hpp"

namespace rythe::rendering
{
	std::unordered_map<std::string, std::unique_ptr<material>> MaterialCache::m_materials;

	material_handle MaterialCache::loadMaterial(const std::string& name, const shader_handle shader)
	{
		if (m_materials.contains(name))
		{
			log::warn("Material {} already exists, ignoring creation request and returning existing material", name);
			return { m_materials[name].get() };
		}

		auto mat = m_materials.emplace(name, std::make_unique<material>()).first->second.get();
		mat->shader = shader;
		mat->name = name;
		return { mat };
	}
	material_handle MaterialCache::loadMaterial(const std::string& name, const std::string& shaderName)
	{
		return loadMaterial(name, ShaderCache::getShader(shaderName));
	}
	material_handle MaterialCache::loadMaterialFromFile(const std::string& name, const std::string& shaderPath)
	{
		shader_handle shader = ShaderCache::createShader(shaderPath);

		return loadMaterial(name, shader);
	}
	material_handle MaterialCache::getMaterial(const std::string& name)
	{
		if (m_materials.contains(name))
		{
			return { m_materials[name].get() };
		}
		return material_handle();
	}
	void MaterialCache::deleteMaterial(const std::string& name)
	{
		if (m_materials.contains(name))
		{
			m_materials[name]->shader->clearBuffers();
			//add a similar func for textures
			m_materials.erase(name);
		}
	}
	std::vector<material_handle> MaterialCache::getMaterials()
	{
		std::vector<material_handle> handles;
		for (auto& [id, handle] : m_materials)
		{
			handles.push_back(material_handle{ handle.get() });
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