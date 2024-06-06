#pragma once
#include <string>
#include <memory>
#include <unordered_map>

//INcludes for async
#include <chrono>
#include <functional>
#include <thread>
#include <future>

#include "core/assets/assethandle.hpp"
#include "core/assets/assetcache.hpp"
#include "graphics/data/shadersource.hpp"
#include "graphics/data/shaderhandle.hpp"
#include "graphics/interface/definitions/shader.hpp"

namespace fs = std::filesystem;
namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	class ShaderCache
	{
	private:
		static std::unordered_map<rsl::id_type, std::unique_ptr<shader>> m_shaders;
		static std::unordered_map<rsl::id_type, std::string> m_names;

	public:
		static shader_handle createShader(const std::string& name, ast::asset_handle<shader_source> shaderSource);
		static void createShaders(std::vector<ast::asset_handle<shader_source>> assets);
		static void compileShaders(bool async = false);
		static shader_handle getShader(const std::string& name);
		static shader_handle getShader(rsl::id_type nameHash);
		static void deleteShader(const std::string& name);
		static void deleteShader(rsl::id_type nameHash);
		static void deleteShaders();
		static std::vector<shader_handle> getShaders();
		static std::vector<std::string> getShaderNames();
		static std::vector<const char*> getShaderNamesC();
	};
}