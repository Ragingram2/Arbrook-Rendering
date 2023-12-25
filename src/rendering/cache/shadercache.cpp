#include "rendering/cache/shadercache.hpp"

namespace rythe::rendering
{
	std::unordered_map<std::string, std::unique_ptr<shader>> ShaderCache::m_shaders;
	std::unordered_map<std::string, std::string> ShaderCache::m_filePaths;

	shader_handle ShaderCache::createShader(const std::string& filepath)
	{
		std::string name = filepath.substr(filepath.rfind('/')+1);
		int extCount = filepath.substr(filepath.rfind('.')).length();
		name = name.substr(0,name.length()-extCount);
		return createShader(name, filepath);
	}
	shader_handle ShaderCache::createShader(const std::string& name, const std::string& filepath)
	{
		if (m_shaders.contains(name))
		{
			return m_shaders[name].get();
		}
		m_filePaths.emplace(name, filepath);
		auto shad = m_shaders.emplace(name, std::make_unique<shader>()).first->second.get();

		shad->m_impl.initialize(name, loadShader(filepath));
		return { shad };
	}
	shader_handle ShaderCache::getShader(const std::string& name)
	{
		if (m_shaders.contains(name))
		{
			return m_shaders[name].get();
		}
		log::warn("Shader {} does not exist", name);
		return shader_handle{};
	}
	void ShaderCache::deleteShader(const std::string& name)
	{
		if (m_shaders.contains(name))
		{
			m_shaders[name]->release();
			m_shaders.erase(name);
		}

		if (m_filePaths.contains(name))
		{
			m_filePaths.erase(name);
		}
	}
	void ShaderCache::reloadShaders()
	{
		for (auto& [name, shader] : m_shaders)
		{
			//Make sure to delete the previous shader on the gpu
			shader->release();
			log::debug("reloading {} at path {} ", name, m_filePaths[name]);
			shader->m_impl.initialize(name, loadShader(m_filePaths[name]));
		}
		log::debug("Done!");
	}
	void ShaderCache::loadShaders(const std::string& directory)
	{
		for (auto& p : fs::directory_iterator(directory))
		{
			if (!p.path().has_extension()) continue;
			if (p.path().extension() != ".shader") continue;

			auto fileName = p.path().stem().string();
			auto path = p.path().string();
			m_filePaths.emplace(fileName, path);
			log::debug("Loading shader {} at \"{}\"", fileName, path);
			createShader(fileName, path);
		}
	}
	//Think about this later
	shader_source ShaderCache::loadShader(const std::string& filepath)
	{
		std::ifstream stream(filepath);
		std::filesystem::directory_entry path{ filepath };

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

		while (getline(stream, line))
		{
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
					log::error("No Shader Type be found or maybe specified type is not supported");
					log::error(line);
					log::error("Stopping parsing");
					break;
				}
				getline(stream, line);
				getline(stream, line);
				parse = true;

			}
			else if (line.find("}//end") != std::string::npos)
			{
				parse = false;
				continue;
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

		return { path.path().stem().string(), ss };
	}


}