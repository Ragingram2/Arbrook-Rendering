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

#include "core/assets/assets.hpp"
#include "graphics/data/mesh.hpp"

namespace fs = std::filesystem;
namespace ast = rythe::core::assets;

namespace rythe::rendering
{
	using mesh_parameters = ast::import_settings<mesh>;
	constexpr mesh_parameters default_mesh_params{};

	class MeshImporter : public ast::AssetImporter<mesh>
	{
	private:
		Assimp::Importer m_importer;
	public:
		virtual bool canLoad(fs::path filePath) override
		{
			return m_importer.IsExtensionSupported(filePath.extension().string().c_str());
		}

		virtual ast::asset_handle<mesh> load(rsl::id_type id, fs::path filePath, mesh* data, const ast::import_settings<mesh>& settings) override
		{
			const aiScene* scene = m_importer.ReadFile(filePath.string(), aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_FlipWindingOrder);
			if (!scene)
			{
				log::error("Problem in loading mesh file");
				return { 0, nullptr };
			}
			data->name = filePath.stem().string();
			data->load(scene);
			return { id, data };
		}


		virtual void free(mesh& asset) override
		{

		}

	};
}