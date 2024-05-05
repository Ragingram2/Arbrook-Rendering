#pragma once
#include <string>
#include <sstream>
#include <utility>
#include <filesystem>

#include <rsl/primitives>
#include <rsl/logging>
#include <nlohmann/json.hpp>

#include "graphics/data/material.hpp"

namespace rythe::rendering
{
	namespace fs = std::filesystem;

	struct material_source
	{
		std::string name;
		std::string shaderName;
		fs::path shaderPath;

		std::unordered_map<std::string, material_parameter_base*> parameters;
	};
}

namespace ns
{
	using json = nlohmann::json;

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(math::vec4, x, y, z, w);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(math::vec3, x, y, z);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(math::vec2, x, y);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(math::ivec4, x, y, z, w);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(math::ivec3, x, y, z);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(math::ivec2, x, y);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(math::uvec4, x, y, z, w);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(math::uvec3, x, y, z);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(math::uvec2, x, y);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(math::bvec4, x, y, z, w);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(math::bvec3, x, y, z);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(math::bvec2, x, y);

	inline void to_json(json& j, const rythe::rendering::material_parameter<rythe::rendering::uniform>& mp)
	{

	}

	inline void from_json(const json& j, rythe::rendering::material_parameter<rythe::rendering::uniform>* mp)
	{
		mp->bufferRegister = j["register"].get<unsigned int>();
		mp->type = rythe::rendering::ParamType::Uniform;
		for (auto elem : j["value"])
		{
			if (elem["type"] == "Scalar")
			{
				mp->name = elem["name"];
				if (elem["value"].is_number_float())
					mp->value.data = elem["value"].get<float>();
				else if (elem["value"].is_number_integer())
					mp->value.data = elem["value"].get<int>();
				else if (elem["value"].is_number_unsigned())
					mp->value.data = elem["value"].get<unsigned int>();
				else
					rythe::log::debug("Element \"{}\", not supported", elem.dump());
			}
			else if (elem["value"].is_object())
			{
				auto val = elem["value"];
				mp->name = elem["name"];
				if (val["type"] == "Float4")
				{
					math::vec4 value;
					from_json(val["value"], value);
					mp->value.data = value;
				}
				if (val["type"] == "Float3")
				{
					math::vec3 value;
					from_json(val["value"], value);
					mp->value.data = value;
				}
				if (val["type"] == "Float2")
				{
					math::vec2 value;
					from_json(val["value"], value);
					mp->value.data = value;
				}
			}
			else
				rythe::log::debug("Element \"{}\", not supported", elem.dump());
		}
	}

	inline void to_json(json& j, const rythe::rendering::material_parameter<std::string>& mp)
	{

	}
	inline void from_json(const json& j, rythe::rendering::material_parameter<std::string>* mp)
	{
		mp->bufferRegister = j["register"].get<unsigned int>();
		mp->type = rythe::rendering::ParamType::Texture;
		auto valueString = j["value"].get<std::string>();
		mp->value = valueString;
	}

	inline void to_json(json& j, const rythe::rendering::material_source& m)
	{
		j["shaderPath"] = m.shaderPath.string();
		json parameterList;
		for (auto& [key, val] : m.parameters)
		{
			json parameter;
			if (val->type == rythe::rendering::ParamType::Texture)
				to_json(parameter, *static_cast<rythe::rendering::material_parameter<std::string>*>(val));
			else if (val->type == rythe::rendering::ParamType::Uniform)
				to_json(parameter, *static_cast<rythe::rendering::material_parameter<rythe::rendering::uniform>*>(val));
			parameterList.push_back(parameter);
		}
		j["parameters"] = parameterList;
	}

	inline void from_json(const json& j, rythe::rendering::material_source& m)
	{
		m.shaderPath = fs::path(j["shaderPath"]);
		m.shaderName = m.shaderPath.stem().string();
		for (auto [key, param] : j["parameters"].items())
		{	
			if (param["type"] == "Texture")
			{
				if (!m.parameters.contains(key))
					m.parameters.emplace(key, new rythe::rendering::material_parameter<std::string>);
				from_json(param, static_cast<rythe::rendering::material_parameter<std::string>*>(m.parameters[key]));
			}
			else 	if (param["type"] == "Uniform")
			{
				if (!m.parameters.contains(key))
					m.parameters.emplace(key, new rythe::rendering::material_parameter<rythe::rendering::uniform>);
				from_json(param, static_cast<rythe::rendering::material_parameter<rythe::rendering::uniform>*>(m.parameters[key]));
			}
		}
	}
}