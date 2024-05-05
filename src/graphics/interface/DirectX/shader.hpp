#pragma once
#include <unordered_map>

#include <rsl/logging>

#include "core/utils/profiler.hpp"

#include "graphics/data/bufferhandle.hpp"
#include "graphics/data/shadersource.hpp"
#include "graphics/cache/windowprovider.hpp"
#include "graphics/cache/buffercache.hpp"
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
		DXVertexShader m_VS = nullptr;
		DXPixelShader m_PS = nullptr;
		DXGeometryShader m_GS = nullptr;
		DXHullShader m_HS = nullptr;
		DXComputeShader m_CS = nullptr;
		std::unordered_map<std::string, buffer_handle> m_constBuffers;
		window_handle m_windowHandle;
	public:

		unsigned int programId;
		std::string name;

		DXBlob VS = nullptr;
		DXBlob PS = nullptr;
		DXBlob GS = nullptr;
		DXBlob HS = nullptr;
		DXBlob CS = nullptr;

		shader() = default;
		shader(shader* other)
		{
			programId = other->programId;
			name = other->name;
		}
		~shader()
		{
			if (m_VS != nullptr)
				m_VS.ReleaseAndGetAddressOf();
			if (m_GS != nullptr)
				m_GS.ReleaseAndGetAddressOf();
			if (m_PS != nullptr)
				m_PS.ReleaseAndGetAddressOf();
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
				m_windowHandle->dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, m_VS.GetAddressOf());

			if (GS != nullptr)
				m_windowHandle->dev->CreateGeometryShader(GS->GetBufferPointer(), GS->GetBufferSize(), NULL, m_GS.GetAddressOf());

			if (PS != nullptr)
				m_windowHandle->dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, m_PS.GetAddressOf());
		}

		void bind()
		{
			ZoneScopedN("[DX11 Shader] bind()");
			m_windowHandle->devcon->VSSetShader(m_VS.Get(), 0, 0);
			m_windowHandle->devcon->GSSetShader(m_GS.Get(), 0, 0);
			m_windowHandle->devcon->PSSetShader(m_PS.Get(), 0, 0);


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
		void setUniform(const std::string& bufferName, int location, elementType data[])
		{
			ZoneScopedN("[DX11 Shader] setUniform()");
			if (m_constBuffers.count(bufferName) != 0)
			{
				m_constBuffers[bufferName]->bufferData<elementType>(data);
				return;
			}

			auto buffer = BufferCache::getBuffer(bufferName);
			if (buffer == nullptr)
			{
				addBuffer(BufferCache::createConstantBuffer<elementType>(bufferName, location, rendering::UsageType::STATICDRAW));
				m_constBuffers[bufferName]->bufferData<elementType>(data);
				return;
			}
			else
			{
				addBuffer(buffer);
				m_constBuffers[bufferName]->bufferData<elementType>(data);
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