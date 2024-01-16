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
		virtual bool canLoad(fs::path filePath) override
		{
			if (!filePath.has_extension())
			{
				log::warn("File path \"{}\" does not have a file extension", filePath.string());
				return false;
			}

			for (auto format : supportedFormats)
			{
				if (filePath.extension() == format)
					return true;
			}
			return false;
		}

		virtual ast::asset_handle<shader_source> load(rsl::id_type id, fs::path filePath, shader_source* data, const ast::import_settings<shader_source>& settings) override
		{
			auto name = filePath.stem().string();

			*data = loadShader(filePath);

			return { id, data };
		}

		virtual void free(shader_source& asset) override
		{

		}
	private:
		//Think about this later
		shader_source loadShader(fs::path filepath)
		{
			std::ifstream stream(filepath);

			enum shader_type : int
			{
				NONE = -1,
				VERTEX = 0,
				FRAG = 1,
				GEOM = 2,
				HULL = 3,
				COMPUTE = 4,
				COUNT = 5
			};

			std::string line;
			std::stringstream ss[shader_type::COUNT];
			shader_type type = shader_type::NONE;
			bool parse = false;
			int braceCounter = 0;
			while (getline(stream, line))
			{
				if (line.find('{'))
					braceCounter++;
				if (line.find('}'))
					braceCounter--;

				if (line.find("namespace") != std::string::npos && !parse)
				{
					if (line.find("vertex") != std::string::npos)
					{
						type = shader_type::VERTEX;
					}
					else if (line.find("fragment") != std::string::npos)
					{
						type = shader_type::FRAG;
					}
					else if (line.find("geometry") != std::string::npos)
					{
						type = shader_type::GEOM;
					}
					else if (line.find("hull") != std::string::npos)
					{
						type = shader_type::HULL;
					}
					else if (line.find("compute") != std::string::npos)
					{
						type = shader_type::COMPUTE;
					}
					else
					{
						log::error("No ShaderType could be found or specified type is not supported");
						log::error(line);
						log::error("Stopping parsing");
						break;
					}
					getline(stream, line);
					if (line.find('{'))
						braceCounter++;
					if (line.find('}'))
						braceCounter--;
					getline(stream, line);
					if (line.find('{'))
						braceCounter++;
					if (line.find('}'))
						braceCounter--;
					parse = true;

				}
				else if (line.find("}") != std::string::npos && braceCounter == 0)
				{
					parse = false;
					continue;
				}
				else if (braceCounter > 0)
				{
					log::error("Unequal braces, this will lead to incorrect parsing");
				}

				if (parse)
				{
					ss[(int)type] << line << "\n";
				}

				if (stream.eof())
				{
					break;
				}
			}

			return { filepath.stem().string(), filepath, ss };
		}
	};
}