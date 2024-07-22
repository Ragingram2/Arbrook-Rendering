#pragma once

#include <rsl/logging>

#include "core/utils/profiler.hpp"

#include "graphics/cache/windowprovider.hpp"
#include "graphics/interface/DirectX/dx11includes.hpp"
#include "graphics/interface/definitions/window.hpp"
#include "graphics/interface/config.hpp"
#include EnumTypes_HPP_PATH


namespace rythe::rendering
{
	struct buffer_handle;
	template<typename APIType>
	struct Ibuffer;

	namespace internal
	{
		struct inputlayout;
	}
}

namespace rythe::rendering::internal
{
	namespace log = rsl::log;
	struct buffer
	{
		friend struct rendering::buffer_handle;
		friend struct Ibuffer<internal::buffer>;
		friend struct internal::inputlayout;
	public:
		unsigned int bindId;
		unsigned int id = 0;
		std::string name;
	private:
		unsigned int m_size;
		unsigned int m_elementSize = 0;

		DXBuffer m_internalBuffer;
		D3D11_BUFFER_DESC m_bufferDesc;

		BufferType m_type;
		UsageType m_usage;
		window_handle m_windowHandle;

	public:
		operator DXBuffer () const { return m_internalBuffer; }
		template<typename elementType>
		void initialize(BufferType target, UsageType usage, int size, elementType data[] = nullptr)
		{
			ZoneScopedN("[DX11 Buffer] initialize()");
			m_windowHandle = WindowProvider::activeWindow;
			m_type = target;
			m_usage = usage;
			m_size = size;
			m_elementSize = sizeof(elementType);

			createBuffer(data);
		}

		void bind()
		{
			ZoneScopedN("[DX11 Buffer] bind()");
			unsigned int offset = 0;
			switch (m_type)
			{
			case BufferType::VERTEX_BUFFER:
				WindowProvider::activeWindow->devcon->IASetVertexBuffers(bindId, 1, m_internalBuffer.GetAddressOf(), &m_elementSize, &offset);
				break;
			case BufferType::INDEX_BUFFER:
				WindowProvider::activeWindow->devcon->IASetIndexBuffer(m_internalBuffer.Get(), static_cast<DXGI_FORMAT>(FormatType::R32U), offset);
				break;
			case BufferType::CONSTANT_BUFFER:
				WindowProvider::activeWindow->devcon->VSSetConstantBuffers(bindId, 1, m_internalBuffer.GetAddressOf());
				WindowProvider::activeWindow->devcon->GSSetConstantBuffers(bindId, 1, m_internalBuffer.GetAddressOf());
				WindowProvider::activeWindow->devcon->PSSetConstantBuffers(bindId, 1, m_internalBuffer.GetAddressOf());
				break;
			default:
				log::error("That type is not supported");
				break;
			}
		}

		void unbind()
		{
			ZoneScopedN("[DX11 Buffer] unbind()");

			/*unsigned int offset = 0;
			switch (m_target)
			{
			case TargetType::VERTEX_BUFFER:
				WindowProvider::activeWindow->devcon->IASetVertexBuffers(bindId, 1, nullptr, 0, &offset);
				break;
			case TargetType::INDEX_BUFFER:
				WindowProvider::activeWindow->devcon->IASetIndexBuffer(nullptr, static_cast<DXGI_FORMAT>(FormatType::R32U), offset);
				break;
			case TargetType::CONSTANT_BUFFER:
				WindowProvider::activeWindow->devcon->VSSetConstantBuffers(bindId, 1, nullptr);
				WindowProvider::activeWindow->devcon->PSSetConstantBuffers(bindId, 1, nullptr);
				break;
			default:
				log::error("That type is not supported");
				break;
			}*/
		}

		template<typename elementType>
		void bufferData(elementType* data, int size = 0)
		{
			ZoneScopedN("[DX11 Buffer] bufferData()");

			if (size < 1)
			{
				size = m_size;
			}

			if (size != m_size || m_internalBuffer == nullptr)
			{
				m_size = size;
				m_elementSize = sizeof(elementType);
				createBuffer(data);
				return;
			}

			bind();

			D3D11_MAPPED_SUBRESOURCE resource;
			ZeroMemory(&resource, sizeof(resource));
			CHECKERROR(WindowProvider::activeWindow->devcon->Map(m_internalBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &resource), "Buffer Failed to be filled", WindowProvider::activeWindow->checkError());
			memcpy(resource.pData, data, m_size * sizeof(elementType));
			WindowProvider::activeWindow->devcon->Unmap(m_internalBuffer.Get(), NULL);
		}

		void release()
		{
			ZoneScopedN("[DX11 Buffer] release()");
			if (m_internalBuffer != nullptr)
			{
				log::debug("[{}] Release buffer \"{}\"", name, static_cast<void*>(m_internalBuffer.Get()));
				m_internalBuffer.Reset();
			}
		}

	private:
		template<typename elementType>
		void createBuffer(elementType* data)
		{
			ZoneScopedN("[DX11 Buffer] createBuffer()");
			if (m_internalBuffer != nullptr)
				m_internalBuffer.Reset();
			ZeroMemory(&m_bufferDesc, sizeof(m_bufferDesc));

			m_bufferDesc.Usage = static_cast<D3D11_USAGE>(m_usage);
			m_bufferDesc.BindFlags = static_cast<D3D11_BIND_FLAG>(m_type);
			m_bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			m_bufferDesc.MiscFlags = 0;
			m_bufferDesc.StructureByteStride = 0;

			if (m_type == BufferType::CONSTANT_BUFFER)
				m_bufferDesc.ByteWidth = static_cast<unsigned int>(m_elementSize + (16 - (m_elementSize % 16)));
			else
				m_bufferDesc.ByteWidth = m_elementSize;

			m_bufferDesc.ByteWidth *= m_size;

			if (data == nullptr)
			{
				ZoneScopedN("[DX11 Buffer][createBuffer()] creating the buffer with null data");
				CHECKERROR(WindowProvider::activeWindow->dev->CreateBuffer(&m_bufferDesc, NULL, m_internalBuffer.GetAddressOf()), "Buffer failed to be created", WindowProvider::activeWindow->checkError())
			}
			else
			{
				ZoneScopedN("[DX11 Buffer][createBuffer()] creating the buffer with data");
				D3D11_SUBRESOURCE_DATA m_initData;

				ZeroMemory(&m_initData, sizeof(m_initData));
				m_initData.pSysMem = data;
				m_initData.SysMemPitch = 0;
				m_initData.SysMemSlicePitch = 0;

				CHECKERROR(WindowProvider::activeWindow->dev->CreateBuffer(&m_bufferDesc, &m_initData, m_internalBuffer.GetAddressOf()), "Buffer failed to be created", WindowProvider::activeWindow->checkError())
			}

			log::debug("[{}] Creating buffer \"{}\"", name, static_cast<void*>(m_internalBuffer.Get()));
		}
	};
}