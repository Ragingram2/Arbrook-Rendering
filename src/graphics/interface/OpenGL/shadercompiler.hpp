#pragma once
#include <filesystem>
#include <iostream>
#include <fstream>

#include <rsl/logging>
#include <rsl/primitives>

#include "core/utils/profiler.hpp"

#include "graphics/data/shadersource.hpp"
#include "graphics/cache/windowprovider.hpp"
#include "graphics/interface/config.hpp"
#include "graphics/interface/OpenGL/glslangincludes.hpp"

namespace rythe::rendering::internal
{
	class ShaderCompiler
	{
	private:
		static DireStackFileIncluder m_includer;
		static glslang::EShClient m_client;
		static glslang::EShTargetClientVersion m_clientVersion;
		static glslang::EShTargetLanguage m_targetLanguage;
		static glslang::EShTargetLanguageVersion m_targetVersion;
		static glslang::EShSource m_sourceType;

		static bool m_initialized;
		static window_handle m_windowHandle;
		static std::vector<std::string> IncludeDirectoryList;
		static std::string glslSource;
	public:
		static void initialize()
		{
			ZoneScopedN("[OpenGL ShaderCompiler] initialized()");
			if (m_initialized) return;
			m_client = glslang::EShClientVulkan;
			m_clientVersion = glslang::EShTargetVulkan_1_0;

			m_targetLanguage = glslang::EShTargetSpv;
			m_targetVersion = glslang::EShTargetSpv_1_0;

			m_sourceType = glslang::EShSourceHlsl;

			m_includer.pushExternalLocalDirectory("resources\\shaders\\include");

			m_windowHandle = WindowProvider::activeWindow;
			m_initialized = true;
		}

		static unsigned int compile(ShaderType type, shader_source source)
		{
			ZoneScopedN("[OpenGL ShaderCompiler] compile()");

			EShLanguage profile;
			int shaderIdx = 0;
			std::string shaderType;
			switch (static_cast<ShaderType>(type))
			{
			case ShaderType::VERTEX:
				profile = EShLangVertex;
				shaderIdx = 0;
				shaderType = "vertex";
				break;
			case ShaderType::FRAGMENT:
				profile = EShLangFragment;
				shaderIdx = 1;
				shaderType = "fragment";
				break;
			case ShaderType::GEOMETRY:
				profile = EShLangGeometry;
				shaderIdx = 2;
				shaderType = "geometry";
				break;
			case ShaderType::HULL:
				profile = EShLangTessControl;
				shaderIdx = 3;
				shaderType = "tesselation-control";
				break;
			case ShaderType::COMPUTE:
				profile = EShLangCompute;
				shaderIdx = 4;
				break;
			default:
				shaderType = "Unsupported/None";
				break;
			}

			std::string file{ std::format("{}-{}", source.fileName, shaderType) };
			auto hlslSource = source.sources[shaderIdx].second;

			std::vector<unsigned int> spirVBin = compileToSpirV(profile, shaderType, source.fileName, hlslSource);
			if (spirVBin.size() < 1)
			{
				log::error("[{}] SpirV file is too small", file);
				return 0;
			}

			if (!compileToGLSL(spirVBin))
			{
				log::error("[{}] Compiling to GLSL failed", file);
				return 0;
			}

			unsigned int id = glCreateShader(static_cast<GLenum>(type));

			log::info("[{}] Compiling GLSL Shader", file);
			const char* src = glslSource.c_str();
			glShaderSource(id, 1, &src, NULL);
			glCompileShader(id);

			GLint isCompiled = 0;
			glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(id, maxLength, &maxLength, &infoLog[0]);
				log::error("[{}] Shader Compilation Failed", file);
				log::error("[{}] Shader Compilation {}", file, infoLog.data());

				glDeleteShader(id);
				return 0;
			}

			log::info("[{}] GLSL Shader Compilation Success", file);

			return id;
		}

	private:
		static std::vector<unsigned int> compileToSpirV(EShLanguage profile, const std::string& shaderType, const std::string& fileName, const std::string& source)
		{
			ZoneScopedN("[OpenGL ShaderCompiler] compileToSpirV()");
			std::vector<unsigned int> spirVBin;

			EShMessages message = (EShMessages)(EShMsgDefault | EShMsgReadHlsl | EShMsgHlslLegalization | EShMsgSpvRules | EShMsgHlslEnable16BitTypes | EOptionHumanReadableSpv);

			const int defaultVersion = 100;

			char* source_c = const_cast<char*>(source.c_str());

			glslang::InitializeProcess();
			glslang::InitializeProcess();
			glslang::InitializeProcess();
			glslang::FinalizeProcess();
			glslang::FinalizeProcess();

			glslang::TShader* _shader = new glslang::TShader(profile);

			_shader->setStrings(&source_c, 1);
			_shader->setEntryPoint("main");
			//_shader->setAutoMapBindings(true);
			//_shader->setAutoMapLocations(true);
			//_shader->setTextureSamplerTransformMode(EShTextureSamplerTransformMode::EShTexSampTransKeep);
			_shader->setCompileOnly();

			//_shader->setFlattenUniformArrays(true);
			_shader->setHlslIoMapping(true);
			_shader->setDxPositionW(true);

			_shader->setEnvInput(m_sourceType, profile, m_client, defaultVersion);
			_shader->setEnvClient(m_client, m_clientVersion);
			_shader->setEnvTarget(m_targetLanguage, m_targetVersion);
			_shader->setEnvTargetHlslFunctionality1();
			_shader->setEnvInputVulkanRulesRelaxed();

			std::string file = std::format("{}-{}", fileName, shaderType);
			std::string str;
			auto resource = GetResources();
			if (!_shader->preprocess(resource, defaultVersion, ENoProfile, false, false, message, &str, m_includer))
			{
				log::error("[{}] Shader preprocessing failed", file);
				log::error(_shader->getInfoLog());
				return spirVBin;
			}
			//log::info(str);

			if (!_shader->parse(GetResources(), defaultVersion, false, message, m_includer))
			{
				log::error("[{}] Shader Compilation failed", file);
				log::error(_shader->getInfoLog());
				return spirVBin;
			}

			glslang::TProgram program;
			program.addShader(_shader);
			program.buildReflection();

			if (!program.link(message))
			{
				log::error("[{}] Program Linking failed:\n{}", file, program.getInfoLog());
			}
			program.dumpReflection();

			if (auto* i = program.getIntermediate(profile))
			{
				spv::SpvBuildLogger logger;
				glslang::GlslangToSpv(*i, spirVBin, &logger);
				if (logger.getAllMessages().length() > 0)
					log::info("[{}] SpirV Conversion output log: {}", file, logger.getAllMessages());

				//if (!glslang::OutputSpvBin(spirVBin, fileName.c_str()))
				//	log::error("[{}] Output to SpirV bin failed", file);
			}


			glslang::FinalizeProcess();
			delete _shader;
			return spirVBin;
		}

		static bool compileToGLSL(std::vector<unsigned int> spirVBin)
		{
			ZoneScopedN("[OpenGL ShaderCompiler] compileToGLSL()");
			spirv_cross::CompilerGLSL glsl(std::move(spirVBin));

			glsl.build_combined_image_samplers();

			const char* textures[6] =
			{
				"Depth_Stencil",
				"Color0",
				"Texture0",
				"Texture1",
				"Texture2",
				"Texture3"
			};
			auto samplers = glsl.get_combined_image_samplers();
			for (auto& resource : samplers)
			{
				uint32_t binding = glsl.get_decoration(resource.image_id, spv::DecorationBinding);
				//log::info("CombinedID:{}, ImageID:{}, ImageBinding:{},SamplerID:{}", resource.combined_id, resource.image_id, binding, resource.sampler_id);

				glsl.set_decoration(resource.combined_id, spv::DecorationBinding, binding);
				glsl.set_name(resource.combined_id, textures[binding]);
			}

			spirv_cross::CompilerGLSL::Options options;
			options.version = 460;
			options.es = false;
			options.emit_push_constant_as_uniform_buffer = true;

			glsl.set_common_options(options);

			glslSource = glsl.compile();
			return true;
		}
	};
	inline std::vector<std::string> ShaderCompiler::IncludeDirectoryList;
	inline DireStackFileIncluder ShaderCompiler::m_includer;
	inline glslang::EShClient ShaderCompiler::m_client;  // will stay EShClientNone if only validating
	inline glslang::EShTargetClientVersion ShaderCompiler::m_clientVersion;       // not valid until m_client is set
	inline glslang::EShTargetLanguage ShaderCompiler::m_targetLanguage;
	inline glslang::EShTargetLanguageVersion ShaderCompiler::m_targetVersion;     // not valid until m_targetLanguage is set
	inline glslang::EShSource ShaderCompiler::m_sourceType;
	inline bool ShaderCompiler::m_initialized = false;
	inline window_handle ShaderCompiler::m_windowHandle;
	inline std::string ShaderCompiler::glslSource;
}