#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "rendering/data/meshhandle.hpp"

namespace fs = std::filesystem;
namespace rythe::rendering
{
	class MeshCache
	{
	private:
		static Assimp::Importer m_importer;
		static std::unordered_map<std::string, std::unique_ptr<mesh>> m_meshes;
	public:
		//needs import settings
		static mesh_handle loadMesh(const std::string& name, const std::string& filePath);
		static mesh_handle getMesh(const std::string& name);
		static void deleteMesh(const std::string& name);
		static void importMeshes(const std::string& filePath);
	};
}