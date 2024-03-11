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
#include "graphics/data/material.hpp"

namespace fs = std::filesystem;
namespace ast = rythe::core::assets;

namespace rythe::rendering
{
	using material_parameters = ast::import_settings<material>;
	constexpr material_parameters default_mesh_params{};

	class MaterialImporter : public ast::AssetImporter<material>
	{
	private:
		Assimp::Importer m_importer;
	public:
		virtual bool canLoad(fs::path filePath) override;
		virtual ast::asset_handle<material> load(rsl::id_type id, fs::path filePath, material* data, const ast::import_settings<material>& settings) override;
		virtual void free(material& asset) override;
	private:
		void initMat(material* data, const aiMaterial* _mat);
	};
}
