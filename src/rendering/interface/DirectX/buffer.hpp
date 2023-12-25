#pragma once

#include "core/logging/logging.hpp"

#include "rendering/cache/windowprovider.hpp"
#include "rendering/interface/DirectX/dx11includes.hpp"
#include "rendering/interface/definitions/window.hpp"
#include "rendering/interface/config.hpp"
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
		//unsigned int m_offsets;
		unsigned int m_size;
		unsigned int m_elementSize = 0;


		ID3D11Buffer* m_internalBuffer;
		D3D11_BUFFER_DESC m_bufferDesc;

		TargetType m_target;
		UsageType m_usage;
		window_handle m_windowHandle;

	public:
		operator ID3D11Buffer* () const { return m_internalBuffer; }
		template<typename elementType>
		void initialize(TargetType target, UsageType usage, int size, elementType data[] = nullptr)
		{
			m_windowHandle = WindowProvider::get(0);
			m_target = target;
			m_usage = usage;
			m_size = size;
			m_elementSize = sizeof(elementType);

			createBuffer(data);
		}

		void bind()
		{
			unsigned int offset = 0;
			switch (m_target)
			{
			case TargetType::VERTEX_BUFFER:
				m_windowHandle->devcon->IASetVertexBuffers(bindId, 1, &m_internalBuffer, &m_elementSize, &offset);
				break;
			case TargetType::INDEX_BUFFER:
				m_windowHandle->devcon->IASetIndexBuffer(m_internalBuffer, static_cast<DXGI_FORMAT>(FormatType::R32U), offset);
				break;
			case TargetType::CONSTANT_BUFFER:
				m_windowHandle->devcon->VSSetConstantBuffers(bindId, 1, &m_internalBuffer);
				m_windowHandle->devcon->PSSetConstantBuffers(bindId, 1, &m_internalBuffer);
				break;
			default:
				log::error("That type is not supported");
				break;
			}
		}

		template<typename elementType>
		void bufferData(elementType data[], int size = 0)
		{
			//if (m_target == TargetType::INDEX_BUFFER)
			//{
			//	log::warn("Index Buffer is not allowed to be written too, returning without writing");
			//	return;
			//}

			if (size < 1)
			{
				size = m_size;
			}
			else if (size != m_size)
			{
				m_size = size;
				m_elementSize = sizeof(elementType);
				bind();
				createBuffer(data);
				return;
			}

			bind();

			D3D11_MAPPED_SUBRESOURCE resource;
			CHECKERROR(m_windowHandle->devcon->Map(m_internalBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &resource),"Buffer Failed to be filled", m_windowHandle->checkError() );
			memcpy(resource.pData, data, m_size * sizeof(elementType));
			m_windowHandle->devcon->Unmap(m_internalBuffer, NULL);
		}

		void release()
		{
			//do stuff here later
		}

	private:
		template<typename elementType>
		void createBuffer(elementType data[])
		{
			ZeroMemory(&m_bufferDesc, sizeof(m_bufferDesc));

			m_bufferDesc.Usage = static_cast<D3D11_USAGE>(m_usage);
			m_bufferDesc.BindFlags = static_cast<D3D11_BIND_FLAG>(m_target);
			m_bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			m_bufferDesc.MiscFlags = 0;
			m_bufferDesc.StructureByteStride = 0;

			if (m_target == TargetType::CONSTANT_BUFFER)
				m_bufferDesc.ByteWidth = static_cast<unsigned int>(m_elementSize + (16 - (m_elementSize % 16)));
			else
				m_bufferDesc.ByteWidth = m_elementSize;

			m_bufferDesc.ByteWidth *= m_size;

			if (data == nullptr)
			{
				CHECKERROR(m_windowHandle->dev->CreateBuffer(&m_bufferDesc, NULL, &m_internalBuffer), "Buffer failed to be created", m_windowHandle->checkError())
			}
			else
			{
				D3D11_SUBRESOURCE_DATA m_initData;

				ZeroMemory(&m_initData, sizeof(m_initData));
				m_initData.pSysMem = data;
				m_initData.SysMemPitch = 0;
				m_initData.SysMemSlicePitch = 0;

				CHECKERROR(m_windowHandle->dev->CreateBuffer(&m_bufferDesc, &m_initData, &m_internalBuffer), "Buffer failed to be created", m_windowHandle->checkError())
			}
		}
	};
}