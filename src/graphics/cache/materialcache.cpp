#include "graphics/cache/materialcache.hpp"
#include "graphics/cache/shadercache.hpp"
#include "graphics/cache/texturecache.hpp"

#include "graphics/cache/importers/shaderimporter.hpp"
#include "graphics/cache/importers/textureimporter.hpp"

namespace rythe::rendering
{
	std::unordered_map<rsl::id_type, std::unique_ptr<material>> MaterialCache::m_materials;
	std::unordered_map<rsl::id_type, std::string> MaterialCache::m_names;

	ast::asset_handle<material> MaterialCache::loadMaterial(const std::string& name, ast::asset_handle<material_source> source)
	{
		rsl::id_type id = rsl::nameHash(name);
		if (m_materials.contains(id))
		{
			log::warn("Material {} already exists, ignoring creation request and returning existing material", name);
			return { id, m_materials[id].get() };
		}

		std::unique_ptr<material> mat = std::make_unique<material>();
		if (source->shaderID != 0)
		{
			mat->setShader(ShaderCache::getShader(source->shaderID));
		}
		else if (!source->shaderName.empty())
		{
			mat->setShader(ShaderCache::getShader(source->shaderName));
		}
		else
		{
			mat->setShader(ShaderCache::getShader("error"));
		}

		auto slot = TextureSlot::TEXTURE0;
		for (fs::path path : source->textureFilepaths)
		{
			auto texName = path.stem().string();
			mat->addTexture(slot, TextureCache::createTexture2D(ast::AssetCache<texture_source>::createAsset(texName, path, default_texture_import_params)));
			slot++;
		}
		mat->name = name;
		m_materials.emplace(id, std::move(mat));
		m_names.emplace(id, name);
		return { id,  m_materials[id].get() };
	}

	ast::asset_handle<material> MaterialCache::loadMaterial(const std::string& name, shader_handle shader)
	{
		rsl::id_type id = rsl::nameHash(name);
		if (m_materials.contains(id))
		{
			log::warn("Material {} already exists, ignoring creation request and returning existing material", name);
			return { id, m_materials[id].get() };
		}

		std::unique_ptr<material> mat = std::make_unique<material>();
		mat->setShader(shader);
		mat->name = name;
		m_materials.emplace(id, std::move(mat));
		m_names.emplace(id, name);
		return { id,  m_materials[id].get() };
	}

	ast::asset_handle<material> MaterialCache::loadMaterial(const std::string& name, const std::string& shaderName)
	{
		return loadMaterial(name, rsl::nameHash(shaderName));
	}
	ast::asset_handle<material> MaterialCache::loadMaterial(const std::string& name, rsl::id_type shaderId)
	{
		return loadMaterial(name, ast::AssetCache<shader_source>::getAsset(shaderId));
	}
	ast::asset_handle<material> MaterialCache::loadMaterial(const std::string& name, ast::asset_handle<shader_source> shader)
	{
		return loadMaterial(name, ShaderCache::createShader(shader->name, shader));
	}

	ast::asset_handle<material> MaterialCache::loadMaterial(const std::string& name)
	{
		return loadMaterial(name, ast::AssetCache<material_source>::getAsset(name));
	}

	ast::asset_handle<material> MaterialCache::loadMaterialFromFile(const std::string& name, fs::path shaderPath)
	{
		return loadMaterial(name, ast::AssetCache<shader_source>::createAsset(name, shaderPath, default_shader_params, false));
	}

	void MaterialCache::loadMaterials(std::vector<ast::asset_handle<material_source>> assets)
	{
		for (auto source : assets)
		{
			loadMaterial(source->name, source);
		}
	}

	ast::asset_handle<material> MaterialCache::getMaterial(const std::string& name)
	{
		rsl::id_type id = rsl::nameHash(name);
		if (m_materials.contains(id))
		{
			return { id, m_materials[id].get() };
		}
		log::error("Material \"{}\" does not exist", name);
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
			ShaderCache::deleteShader(m_materials[nameHash]->getShader()->getName());
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
		for (auto& [id, name] : m_names)
		{
			names.push_back(name);
		}
		return names;
	}

	std::vector<const char*> MaterialCache::getMaterialNamesC()
	{
		std::vector<const char*> names;
		for (auto& [id, name] : m_names)
		{
			names.push_back(name.c_str());
		}
		return names;
	}
}