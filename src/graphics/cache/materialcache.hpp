#pragma once
#include <string>
#include <memory>
#include <unordered_map>

#include "core/assets/assethandle.hpp"
#include "graphics/data/shaderhandle.hpp"
#include "graphics/data/material.hpp"
#include "graphics/data/materialsource.hpp"

namespace fs = std::filesystem;
namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	using shader_params = ast::import_settings<shader_source>;
	constexpr shader_params default_shader_params{};

	class MaterialCache
	{
	private:
		static std::unordered_map<rsl::id_type, std::unique_ptr<material>> m_materials;
		static std::unordered_map<rsl::id_type, std::string> m_names;
	public:
		//needs import settings
		static ast::asset_handle<material> loadMaterial(const std::string& name, ast::asset_handle<material_source> material);
		static ast::asset_handle<material> loadMaterial(const std::string& name, ast::asset_handle<shader_source> shader);
		static ast::asset_handle<material> loadMaterial(const std::string& name, shader_handle shader);
		static ast::asset_handle<material> loadMaterial(const std::string& name, const std::string& shaderName);
		static ast::asset_handle<material> loadMaterial(const std::string& name, rsl::id_type shaderId);
		static ast::asset_handle<material> loadMaterial(const std::string& name);
		static ast::asset_handle<material> loadMaterialFromFile(const std::string& name, fs::path shaderPath);
		static void loadMaterials(std::vector<ast::asset_handle<material_source>> assets);
		static ast::asset_handle<material> getMaterial(rsl::id_type id);
		static ast::asset_handle<material> getMaterial(const std::string& name);
		static void deleteMaterial(const std::string& name);
		static void deleteMaterial(rsl::id_type nameHash);
		static std::vector<ast::asset_handle<material>> getMaterials();
		static std::vector<std::string> getMaterialNames();
		static std::vector<const char*> getMaterialNamesC();
	};
}