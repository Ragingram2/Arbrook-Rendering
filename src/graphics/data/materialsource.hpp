#pragma once
#include <string>
#include <sstream>
#include <utility>
#include <filesystem>

#include <rsl/primitives>
#include <nlohmann/json.hpp>

#include "graphics/data/material.hpp"

namespace rythe::rendering
{
	namespace fs = std::filesystem;
	struct material_source
	{
		std::string name;
		std::string shaderName;
		fs::path filePath;
		rsl::id_type shaderID = 0;

		std::vector<std::string> textures;
	};
}

namespace ns
{
	using json = nlohmann::json;
	inline void to_json(json& j, const rythe::rendering::material_source& m)
	{
		j["shaderPath"] = m.filePath.string();
		j["shaderID"] = m.shaderID;
		j["textures"] = json(m.textures);
	}

	inline void from_json(const json& j, rythe::rendering::material_source& m)
	{
		m.filePath = fs::path(j["shaderPath"]);
		m.shaderName = m.filePath.stem().string();
		m.shaderID = j["shaderID"];
		m.textures = j["textures"].get<std::vector<std::string>>();
	}
}