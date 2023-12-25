#pragma once
#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "rendering/data/shadersource.hpp"
#include "rendering/data/shaderhandle.hpp"
#include "rendering/interface/definitions/shader.hpp"

namespace fs = std::filesystem;
namespace rythe::rendering
{
	class ShaderCache
	{
	private:
		static std::unordered_map<std::string, std::unique_ptr<shader>> m_shaders;
		static std::unordered_map<std::string, std::string> m_filePaths;
	public:
		static shader_handle createShader(const std::string& filepath);
		static shader_handle createShader(const std::string& name,const std::string& filepath);
		static shader_handle getShader(const std::string& name);
		static void deleteShader(const std::string& name);
		static void reloadShaders();
		static void loadShaders(const std::string& directory);
		static shader_source loadShader(const std::string& filepath);
	};
}