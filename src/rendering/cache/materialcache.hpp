#pragma once
#include <string>
#include <memory>
#include <unordered_map>

#include "rendering/data/materialhandle.hpp"
#include "rendering/data/shaderhandle.hpp"
#include "rendering/data/texturehandle.hpp"

namespace rythe::rendering
{
	class MaterialCache
	{
	private:
		static std::unordered_map<std::string, std::unique_ptr<material>> m_materials;
	public:
		//needs import settings
		static material_handle loadMaterial(const std::string& name, const shader_handle shader);
		static material_handle loadMaterial(const std::string& name, const std::string& shaderName);
		static material_handle loadMaterialFromFile(const std::string& name, const std::string& shaderPath);
		static material_handle getMaterial(const std::string& name);
		static void deleteMaterial(const std::string& name);
		static std::vector<material_handle> getMaterials();
		static std::vector<std::string> getMaterialNames();
		static std::vector<const char*> getMaterialNamesC();
	};
}