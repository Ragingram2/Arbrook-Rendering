#include "graphics/cache/importers/shaderimporter.hpp"

template<>
std::unordered_map<rsl::id_type, std::unique_ptr<rythe::core::assets::AssetImporter<rythe::rendering::shader_source>>> rythe::core::assets::AssetCache<rythe::rendering::shader_source>::m_importers;
template<>
std::unordered_map<rsl::id_type, std::unique_ptr<rythe::rendering::shader_source>> rythe::core::assets::AssetCache<rythe::rendering::shader_source>::m_assets;
template<>
std::unordered_map<rsl::id_type, std::string> rythe::core::assets::AssetCache<rythe::rendering::shader_source>::m_names;

namespace rythe::rendering
{
	bool ShaderImporter::canLoad(fs::path filePath)
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

	ast::asset_handle<shader_source> ShaderImporter::load(rsl::id_type id, fs::path filePath, shader_source* data, const ast::import_settings<shader_source>& settings)
	{
		auto name = filePath.stem().string();

		*data = loadShader(filePath);

		return { id, data };
	}

	ast::asset_handle<shader_source> ShaderImporter::loadFromMemory(rsl::id_type id, shader_source* data, const ast::import_settings<shader_source>& settings)
	{
		return { id,data };
	}

	void ShaderImporter::write(fs::path filePath, shader_source* data)
	{
		//std::ofstream o(filePath);
		//nlohmann::json j;
		//ns::to_json(j, *data);
		//o << std::setw(4) << j << std::endl;
	}

	void ShaderImporter::free(shader_source& asset) { }

	shader_source ShaderImporter::loadShader(fs::path filepath)
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
}