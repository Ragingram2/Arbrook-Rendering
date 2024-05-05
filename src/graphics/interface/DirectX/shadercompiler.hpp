#pragma once
#include <string>
#include <vector>
#include <filesystem>

#include <rsl/logging>
#include <rsl/primitives>

#include "graphics/interface/config.hpp"
#include "graphics/interface/DirectX/dx11includes.hpp"
#include "graphics/interface/DirectX/shaderincluder.hpp"

#include "graphics/cache/windowprovider.hpp"
#include "graphics/data/shadersource.hpp"


namespace rythe::rendering::internal
{
	template<typename T, size_t N>
	T* end(T(&ra)[N]) {
		return ra + N;
	}

	class ShaderCompiler
	{
	private:
		static bool m_initialized;
		static Includer* m_includer;
		static window_handle m_windowHandle;
		static std::vector<std::string> m_profiles;
		static const char* m_profiles_c[6];
	public:
		static void initialize()
		{
			ZoneScopedN("[DX11 Shadercompiler] initialize()");
			if (m_initialized) return;

			m_windowHandle = WindowProvider::activeWindow;
			m_initialized = true;
		}

		static DXBlob compile(ShaderType type, shader_source source)
		{
			ZoneScopedN("[DX11 Shadercompiler] compile()");
			std::string profile = m_profiles[static_cast<int>(type)];
			std::string file{ std::format("{}-{}", source.name, profile) };

			UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
			flags |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
			flags |= D3DCOMPILE_AVOID_FLOW_CONTROL;
#if defined( DEBUG ) || defined( _DEBUG )
			flags |= D3DCOMPILE_DEBUG;
			flags |= D3DCOMPILE_OPTIMIZATION_LEVEL0;
#else
			flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

			DXBlob errors;
			DXBlob ppShaderBlob;
			std::string src = source.sources[static_cast<int>(type)].second;
			std::string sourceName = std::format("{}.shader", source.name);

			if (src.empty())
			{
				return nullptr;
			}
			src = std::string("#define DirectX\n").append(src);

			log::info("[{}] Preproccessing Shader", file);
			CHECKERROR(D3DPreprocess(src.c_str(), src.length(), sourceName.c_str(), nullptr, m_includer, &ppShaderBlob, &errors), "Shader failed to preprocess", m_windowHandle->checkError());

			if (errors && errors->GetBufferSize())
			{
				auto infoLog = static_cast<char*>(errors->GetBufferPointer());
				log::error("[{}] Shader Preproccesing Failed", file);
				log::error("[{}] Shader Preproccesing log:\n{}", file, infoLog);
				return nullptr;
			}
			else
				log::info("[{}] Shader Preprocesing Success", file);

			errors = nullptr;

			DXBlob shaderBlob;
			log::info("[{}] Compiling Shader", file);
			CHECKERROR(D3DCompile(ppShaderBlob->GetBufferPointer(), ppShaderBlob->GetBufferSize(), sourceName.c_str(), nullptr, m_includer, "main", profile.c_str(), flags, 0, &shaderBlob, &errors), std::format("Shader failed to compile:\n{}", static_cast<char*>(errors->GetBufferPointer())), m_windowHandle->checkError(););


			//It really sucks that I can't differentiate different warning types
			if (errors && errors->GetBufferSize())
			{
				auto infoLog = static_cast<char*>(errors->GetBufferPointer());
				log::error("[{}] Shader Compilation Failed", file);
				log::error("[{}] Shader Compilation log:\n{}", file, infoLog);
				return nullptr;
			}
			else
				log::info("[{}] Shader Compilation Success", file);

			return shaderBlob;
		}
	};

	inline bool ShaderCompiler::m_initialized = false;
	inline Includer* ShaderCompiler::m_includer = new Includer("resources/shaders/");
	inline window_handle ShaderCompiler::m_windowHandle;
	inline const char* ShaderCompiler::m_profiles_c[6] = { "vs_4_0", "ps_4_0", "gs_4_0","hs_4_0", "ds_4_0","cs_4_0" };
	inline std::vector<std::string> ShaderCompiler::m_profiles(m_profiles_c, end(m_profiles_c));
}