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
#include <nlohmann/json.hpp>

#include "core/assets/assets.hpp"
#include "graphics/data/materialsource.hpp"

namespace fs = std::filesystem;
namespace ast = rythe::core::assets;

namespace rythe::rendering
{
	using material_parameters = ast::import_settings<material_source>;
	constexpr material_parameters default_material_params{};

	class MaterialImporter : public ast::AssetImporter<material_source>
	{
	private:
		Assimp::Importer m_importer;
		static constexpr const char* supportedFormats[] = { ".mat" };
	public:
		virtual bool canLoad(fs::path filePath) override;
		virtual ast::asset_handle<material_source> load(rsl::id_type id, fs::path filePath, material_source* data, const ast::import_settings<material_source>& settings) override;
		virtual ast::asset_handle<material_source> loadFromMemory(rsl::id_type id, material_source* data, const ast::import_settings<material_source>& settings) override;
		virtual void write(fs::path filePath, material_source* data) override;
		virtual void free(material_source& asset) override;
	private:
		void initMat(material_source* data, const aiMaterial* _mat);
	};
}
