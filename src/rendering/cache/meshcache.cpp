#include "rendering/cache/meshcache.hpp"

namespace rythe::rendering
{
	Assimp::Importer MeshCache::m_importer;

	std::unordered_map<std::string, std::unique_ptr<mesh>> MeshCache::m_meshes;

	mesh_handle MeshCache::loadMesh(const std::string& name, const std::string& filePath)
	{
		if (m_meshes.contains(name))
		{
			log::warn("Mesh {} already loaded, ignoring new load request and returning existing mesh", name);
			return m_meshes[name].get();
		}

		const aiScene* scene = m_importer.ReadFile(filePath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_FlipWindingOrder);
		auto mes = m_meshes.emplace(name, std::make_unique<mesh>()).first->second.get();
		mes->name = name;
		mes->load(scene);
		return { mes };
	}

	mesh_handle MeshCache::getMesh(const std::string& name)
	{
		if (m_meshes.contains(name))
		{
			return m_meshes[name].get();
		}
		log::warn("Mesh {} does not exist", name);
		return mesh_handle{};
	}

	void MeshCache::deleteMesh(const std::string& name)
	{
		if (m_meshes.contains(name))
		{
			m_meshes.erase(name);
		}
	}

	void MeshCache::importMeshes(const std::string& filePath)
	{
		for(auto& p : fs::recursive_directory_iterator(filePath))
		{
			loadMesh(p.path().filename().string(), p.path().stem().string());
		}
	}
}