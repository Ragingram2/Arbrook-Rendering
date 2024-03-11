#include "graphics/cache/modelcache.hpp"
#include "graphics/cache/materialcache.hpp"
#include "graphics/cache/importers/meshimporter.hpp"


namespace fs = std::filesystem;
namespace ast = rythe::core::assets;

//instead of making this an importer, use this as a model factory, and use the mesh importer here
namespace rythe::rendering
{
	std::unordered_map<rsl::id_type, std::unique_ptr<model>> ModelCache::m_models;
	std::unordered_map<rsl::id_type, std::string> ModelCache::m_names;

	ast::asset_handle<model> ModelCache::createModel(const std::string& name, ast::asset_handle<mesh> meshHandle, ast::asset_handle<material> matHandle)
	{
		if (!matHandle)
		{
			matHandle = MaterialCache::getMaterial("error");
		}

		rsl::id_type id = rsl::nameHash(name);

		if (m_models.contains(id))
		{
			log::warn("Model {} already exists, ignoring new model, and returning existing one", name);
			return { id, m_models[id].get() };
		}

		m_names.emplace(id, name);
		auto mod = m_models.emplace(id, std::make_unique<model>()).first->second.get();
		mod->meshHandle = meshHandle;
		//mod->matHandle = matHandle;
		mod->name = name;
		return { id, mod };
	}
	ast::asset_handle<model> ModelCache::getModel(const std::string& name)
	{
		ZoneScopedN("Get Model(String)");
		return getModel(rsl::nameHash(name));
	}
	ast::asset_handle<model> ModelCache::getModel(rsl::id_type nameHash)
	{
		ZoneScopedN("Get Model(NameHash)");
		if (m_models.contains(nameHash))
		{
			return { nameHash, m_models[nameHash].get() };
		}
		log::warn("Model {} does not exist", m_names[nameHash]);
		return { 0, nullptr };
	}
	void ModelCache::deleteModel(const std::string& name)
	{
		deleteModel(rsl::nameHash(name));
	}
	void ModelCache::deleteModel(rsl::id_type nameHash)
	{
		if (m_models.contains(nameHash))
		{
			m_models.erase(nameHash);
			m_names.erase(nameHash);
		}
	}
	void ModelCache::loadModels(std::vector<ast::asset_handle<mesh>> meshes)
	{
		for (auto& handle : meshes)
		{
			createModel(handle->name, handle, {0,nullptr});
		}
	}
	std::vector<ast::asset_handle<model>> ModelCache::getModels()
	{
		std::vector<ast::asset_handle<model>> handles;
		for (auto& [id, data] : m_models)
		{
			handles.emplace_back(id,*data.get());
		}
		return handles;
	}
	std::vector<std::string> ModelCache::getModelNames()
	{
		std::vector<std::string> names;
		for (auto& [id, name] : m_names)
		{
			names.push_back(name);
		}
		return names;
	}
	std::vector<const char*> ModelCache::getModelNamesC()
	{
		std::vector<const char*> names;
		for (auto& [id, name] : m_names)
		{
			names.push_back(name.c_str());
		}
		return names;
	}
}