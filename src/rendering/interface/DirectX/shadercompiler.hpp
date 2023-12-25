#pragma once
#include <string>
#include <vector>
#include <filesystem>

#include <rsl/logging>
#include <rsl/primitives>

#include "rendering/interface/config.hpp"
#include "rendering/interface/DirectX/dx11includes.hpp"
#include "rendering/interface/DirectX/shaderincluder.hpp"

#include "rendering/cache/windowprovider.hpp"
#include "rendering/data/shadersource.hpp"


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
			if (m_initialized) return;

			m_windowHandle = WindowProvider::get(0);
			m_initialized = true;
		}

		static ID3DBlob* compile(ShaderType type, shader_source source)
		{
			std::string profile = m_profiles[static_cast<int>(type)];
			std::string file{ std::format("{}-{}", source.fileName, profile) };

			UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
			flags |= D3DCOMPILE_DEBUG;
			flags |= D3DCOMPILE_OPTIMIZATION_LEVEL0;
#elif
			flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

			ID3DBlob* errors;
			ID3DBlob* ppShaderBlob;
			std::string src = source.sources[static_cast<int>(type)].second;
			std::string sourceName = std::format("{}.shader", source.fileName);

			log::debug("[{}] Preproccessing Shader", file);
			CHECKERROR(D3DPreprocess(src.c_str(), src.length(), sourceName.c_str(), nullptr, m_includer, &ppShaderBlob, &errors), "Shader failed to preprocess", m_windowHandle->checkError());

			if (errors && errors->GetBufferSize())
				rsl::log::error("[{}] Shader Preprocesing {}", file, static_cast<char*>(errors->GetBufferPointer()));
			else
				rsl::log::debug("[{}] Shader Preprocesing Success", file);

			errors = nullptr;

			ID3DBlob* shaderBlob;
			rsl::log::debug("[{}] Compiling Shader", file);
			CHECKERROR(D3DCompile(ppShaderBlob->GetBufferPointer(), ppShaderBlob->GetBufferSize(), sourceName.c_str(), nullptr, m_includer, "main", profile.c_str(), flags, 0, &shaderBlob, &errors), "Shader failed to compile", m_windowHandle->checkError());


			if (errors && errors->GetBufferSize())
				rsl::log::error("[{}] Shader Compilation {}", file, static_cast<char*>(errors->GetBufferPointer()));
			else
				rsl::log::debug("[{}] Shader Compilation Success", file);

			return std::move(shaderBlob);
		}
	};

	inline bool ShaderCompiler::m_initialized = false;
	inline Includer* ShaderCompiler::m_includer = new Includer("resources/shaders/");
	inline window_handle ShaderCompiler::m_windowHandle;
	inline const char* ShaderCompiler::m_profiles_c[6] = { "vs_4_0", "ps_4_0", "gs_4_0","hs_4_0", "ds_4_0","cs_4_0" };
	inline std::vector<std::string> ShaderCompiler::m_profiles(m_profiles_c, end(m_profiles_c));
}