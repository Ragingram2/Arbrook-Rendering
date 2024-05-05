#pragma once
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
#include "graphics/data/materialsource.hpp"

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
		virtual bool canLoad(fs::path filePath) override;
		virtual ast::asset_handle<mesh> load(rsl::id_type id, fs::path filePath, mesh* data, const ast::import_settings<mesh>& settings) override;
		virtual ast::asset_handle<mesh> loadFromMemory(rsl::id_type id, mesh* data, const ast::import_settings<mesh>& settings) override;
		virtual void write(fs::path filePath, mesh* data) override;
		virtual void free(mesh& asset) override;
	private:
		void initMesh(mesh* data, const aiMesh* _mesh);
		std::vector<material_parameter<std::string>*> initMaterial(const aiScene* scene, aiMaterial* mat, aiTextureType type, const std::string& defaultTexName);
	};
}