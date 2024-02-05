#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <rsl/utilities>

#include "core/utils/profiler.hpp"

#include "core/assets/assets.hpp"
#include "graphics/data/model.hpp"
#include "graphics/data/mesh.hpp"

namespace fs = std::filesystem;
namespace ast = rythe::core::assets;

namespace rythe::rendering
{
	class ModelCache
	{
	private:
		static std::unordered_map<rsl::id_type, std::unique_ptr<model>> m_models;
		static std::unordered_map<rsl::id_type, std::string> m_names;
	public:
		static ast::asset_handle<model> createModel(const std::string& name, ast::asset_handle<mesh> meshHandle, ast::asset_handle<material> matHandle);
		static ast::asset_handle<model> getModel(const std::string& name);
		static ast::asset_handle<model> getModel(rsl::id_type nameHash);
		static void deleteModel(const std::string& name);
		static void deleteModel(rsl::id_type nameHash);
		static void loadModels(std::vector<ast::asset_handle<mesh>> meshes);
		static std::vector<ast::asset_handle<model>> getModels();
		static std::vector<std::string> getModelNames();
		static std::vector<const char*> getModelNamesC();
	};
}