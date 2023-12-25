#include "rendering/cache/modelcache.hpp"
#include "rendering/cache/meshcache.hpp"

namespace rythe::rendering
{
	std::unordered_map<rsl::id_type, std::unique_ptr<model>> ModelCache::m_models;

	model_handle ModelCache::createModel(const std::string& name, const std::string& filePath)
	{
		rsl::id_type id = rsl::nameHash(name);

		if (m_models.contains(id))
		{
			log::warn("Model {} already exists, ignoring new model, and returning existing one", name);
			return { m_models[id].get() };
		}

		auto mod = m_models.emplace(id, std::make_unique<model>()).first->second.get();
		mod->meshHandle = MeshCache::loadMesh(name, filePath);
		mod->name = name;
		return { mod };
	}
	model_handle ModelCache::createModel(const std::string& name, mesh_handle handle)
	{
		rsl::id_type id = rsl::nameHash(name);

		if (m_models.contains(id))
		{
			log::warn("Model {} already exists, ignoring new model, and returning existing one", name);
			return { m_models[id].get() };
		}
		auto mod = m_models.emplace(id, std::make_unique<model>()).first->second.get();
		mod->meshHandle = handle;
		mod->name = name;
		return { mod };
	}
	model_handle ModelCache::getModel(const std::string& name)
	{
		rsl::id_type id = rsl::nameHash(name);

		if (m_models.contains(id))
		{
			return { m_models[id].get() };
		}
		log::warn("Model {} does not exist", name);
		return model_handle{};
	}
	void ModelCache::deleteModel(const std::string& name)
	{
		rsl::id_type id = rsl::nameHash(name);

		if (m_models.contains(id))
		{
			m_models.erase(id);
		}
	}
	void ModelCache::loadModels(const std::string& directory)
	{
		for (auto& p : fs::directory_iterator(directory))
		{
			if (!p.path().has_extension()) continue;
			auto fileName = p.path().stem().string();
			auto path = p.path().string();
			log::debug("Loading model {} at \"{}\"",fileName, path);
			createModel(fileName, path);
		}
	}
	std::vector<model_handle> ModelCache::getModels()
	{
		std::vector<model_handle> handles;
		for (auto& [id, handle] : m_models)
		{
			handles.push_back(model_handle{ handle.get()});
		}
		return handles;
	}
	std::vector<std::string> ModelCache::getModelNames()
	{
		std::vector<std::string> names;
		for (auto& [id, handle] : m_models)
		{
			names.push_back(handle->name);
		}
		return names;

	}
	std::vector<const char*> ModelCache::getModelNamesC()
	{
		std::vector<const char*> names;
		for (auto& [id, handle] : m_models)
		{
			names.push_back(handle->name.c_str());
		}
		return names;
	}
}