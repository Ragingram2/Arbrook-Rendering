#pragma once
#include <unordered_map>

#include <rsl/logging>

#include "core/utils/profiler.hpp"

#include "graphics/data/bufferhandle.hpp"
#include "graphics/data/shadersource.hpp"
#include "graphics/cache/windowprovider.hpp"
#include "graphics/interface/DirectX/dx11includes.hpp"
#include "graphics/interface/DirectX/shadercompiler.hpp"
#include "graphics/interface/definitions/window.hpp"
#include "graphics/interface/config.hpp"
#include EnumTypes_HPP_PATH

namespace rythe::rendering
{
	template<typename APIType>
	struct Ishader;
}
namespace rythe::rendering::internal
{
	struct shader
	{
	private:
		ID3D11VertexShader* m_VS = nullptr;
		ID3D11PixelShader* m_PS = nullptr;
		ID3D11GeometryShader* m_GS = nullptr;
		ID3D11HullShader* m_HS = nullptr;
		ID3D11ComputeShader* m_CS = nullptr;
		std::unordered_map<std::string, buffer_handle> m_constBuffers;
		window_handle m_windowHandle;
	public:

		unsigned int programId;
		std::string name;

		ID3DBlob* VS = nullptr;
		ID3DBlob* PS = nullptr;
		ID3DBlob* GS = nullptr;
		ID3DBlob* HS = nullptr;
		ID3DBlob* CS = nullptr;

		shader() = default;
		shader(shader* other)
		{
			programId = other->programId;
			name = other->name;
		}
		~shader()
		{
			if (m_VS != nullptr)
				m_VS->Release();
			if (m_GS != nullptr)
				m_GS->Release();
			if (m_PS != nullptr)
				m_PS->Release();
		}
		operator unsigned int() const { return programId; }

		void initialize(const std::string& shaderName, const shader_source& source)
		{
			ZoneScopedN("[DX11 Shader] initialize()");
			name = shaderName;
			m_windowHandle = WindowProvider::activeWindow;
			ShaderCompiler::initialize();
			VS = ShaderCompiler::compile(ShaderType::VERTEX, source);
			GS = ShaderCompiler::compile(ShaderType::GEOMETRY, source);
			PS = ShaderCompiler::compile(ShaderType::FRAGMENT, source);

			if (VS != nullptr)
				m_windowHandle->dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_VS);

			if (GS != nullptr)
				m_windowHandle->dev->CreateGeometryShader(GS->GetBufferPointer(), GS->GetBufferSize(), NULL, &m_GS);

			if (PS != nullptr)
				m_windowHandle->dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_PS);
		}

		void bind()
		{
			ZoneScopedN("[DX11 Shader] bind()");
			m_windowHandle->devcon->VSSetShader(m_VS, 0, 0);
			m_windowHandle->devcon->GSSetShader(m_GS, 0, 0);
			m_windowHandle->devcon->PSSetShader(m_PS, 0, 0);


			for (auto& [name, handle] : m_constBuffers)
			{
				ZoneScopedN("[DX11 Shader][bind()] binding attached buffers");
				handle->bind();
			}
		}

		void unbind()
		{
			ZoneScopedN("[DX11 Shader] unbind()");
			m_windowHandle->devcon->VSSetShader(nullptr, 0, 0);
			m_windowHandle->devcon->GSSetShader(nullptr, 0, 0);
			m_windowHandle->devcon->PSSetShader(nullptr, 0, 0);

			for (auto& [name, handle] : m_constBuffers)
			{
				ZoneScopedN("[DX11 Shader][unbind()] unbinding attached buffers");
				handle->unbind();
			}
		}

		void addBuffer(buffer_handle handle)
		{
			ZoneScopedN("[DX11 Shader] addBuffer()");
			if (static_cast<internal::BufferType>(handle->getTargetType()) != BufferType::CONSTANT_BUFFER)
			{
				log::error("Buffer is not a constant buffer, this is not supported");
				return;
			}

			if (!m_constBuffers.count(handle->getName()))
				m_constBuffers.emplace(handle->getName(), handle);
		}

		template<typename elementType>
		void setUniform(const std::string& bufferName, elementType data[])
		{
			ZoneScopedN("[DX11 Shader] setUniform()");
			if (m_constBuffers.count(bufferName) != 0)
			{
				m_constBuffers[bufferName]->bufferData(data);
				return;
			}

			log::error("No data was buffered, because the buffer {} was not added or does not exist", bufferName);
		}

		void release()
		{
			ZoneScopedN("[DX11 Shader] release()");
			clearBuffers();
		}

		void clearBuffers()
		{
			ZoneScopedN("[DX11 Shader] clearBuffers()");
			m_constBuffers.clear();
		}
	};
}