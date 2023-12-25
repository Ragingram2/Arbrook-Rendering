#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <rsl/utilities>

#include "rendering/data/meshhandle.hpp"
#include "rendering/data/modelhandle.hpp"

namespace fs = std::filesystem;
namespace rythe::rendering
{
	class ModelCache
	{
	private:
		static std::unordered_map<rsl::id_type, std::unique_ptr<model>> m_models;
	public:
		//needs import settings
		static model_handle createModel(const std::string& name, const std::string& filePath);
		static model_handle createModel(const std::string& name, mesh_handle handle);
		static model_handle getModel(const std::string& name);
		static void deleteModel(const std::string& name);
		static void loadModels(const std::string& directory);
		static std::vector<model_handle> getModels();
		static std::vector<std::string> getModelNames();
		static std::vector<const char*> getModelNamesC();
	};
}