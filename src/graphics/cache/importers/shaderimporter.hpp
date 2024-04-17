#pragma once
#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <rsl/logging>

#include "core/assets/assets.hpp"
#include "graphics/data/shadersource.hpp"
//#include "graphics/interface/definitions/shader.hpp"

namespace fs = std::filesystem;
namespace ast = rythe::core::assets;

namespace rythe::rendering
{
	namespace log = rsl::log;
	class ShaderImporter : public ast::AssetImporter<shader_source>
	{
	private:
		static constexpr const char* supportedFormats[] = { ".shader" };
	public:
		virtual bool canLoad(fs::path filePath) override;
		virtual ast::asset_handle<shader_source> load(rsl::id_type id, fs::path filePath, shader_source* data, const ast::import_settings<shader_source>& settings) override;
		virtual ast::asset_handle<shader_source> loadFromMemory(rsl::id_type id, shader_source* data, const ast::import_settings<shader_source>& settings) override;
		virtual void write(fs::path filePath, shader_source* data) override;
		virtual void free(shader_source& asset) override;
	private:
		//Think about this later
		shader_source loadShader(fs::path filepath);
	};
}