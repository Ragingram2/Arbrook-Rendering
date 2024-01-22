#pragma once
#include <string>

#include <tracy/Tracy.hpp>

#include "graphics/data/textureparameters.hpp"
#include "graphics/cache/windowprovider.hpp"
#include "graphics/interface/DirectX/dx11includes.hpp"
#include "graphics/interface/definitions/window.hpp"

namespace rythe::rendering::internal
{
	struct framebuffer;
	//this should behave more like buffers
	struct texture
	{
		friend struct framebuffer;
	private:
		ID3D11Texture2D* m_texture;
		ID3D11ShaderResourceView* m_shaderResource = nullptr;
		ID3D11SamplerState* m_texSamplerState = nullptr;
		D3D11_SAMPLER_DESC m_sampDesc;
		D3D11_TEXTURE2D_DESC m_texDesc;
		D3D11_BIND_FLAG m_texType;
		D3D11_USAGE m_usageType;
		window_handle m_windowHandle;
	public:
		int channels;
		math::ivec2 resolution;
		unsigned int id;
		std::string name;
		unsigned char* data;
		texture_parameters params;
		texture() = default;
		texture(unsigned int id, std::string name) : id(id), name(name) { }
		texture(texture* other)
		{
			name = other->name;
			id = other->id;
			data = other->data;
			params = other->params;
		}

		void initialize(TargetType texType, texture_parameters _params)
		{
			ZoneScopedN("[DX11 Texture] initialize()");
			m_windowHandle = WindowProvider::activeWindow;
			params = _params;
			m_texType = static_cast<D3D11_BIND_FLAG>(texType);
			m_usageType = static_cast<D3D11_USAGE>(_params.usage);
		}

		void bind(TextureSlot slot)
		{
			ZoneScopedN("[DX11 Texture] bind()");
			m_windowHandle->devcon->PSSetShaderResources(static_cast<UINT>(slot), 1, &m_shaderResource);
			m_windowHandle->devcon->PSSetSamplers(static_cast<UINT>(slot), 1, &m_texSamplerState);
		}

		void unbind()
		{
			ZoneScopedN("[DirectX Texture] unbind()");

		}

		void setMipCount(int mipCount)
		{
			params.mipLevels = mipCount = mipCount >= 0 ? mipCount : 1;
			m_texDesc.MipLevels = mipCount;
		}

		void setWrapMode(int axis, WrapMode mode)
		{
			switch (axis)
			{
			case 0:
				m_sampDesc.AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(mode);
				break;
			case 1:
				m_sampDesc.AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(mode);
				break;
			case 2:
				m_sampDesc.AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(mode);
				break;
			}
		}

		void setMinFilterMode(FilterMode mode)
		{
			params.minFilterMode = static_cast<rendering::FilterMode>(mode);
			m_sampDesc.Filter = static_cast<D3D11_FILTER>(mode);
		}

		void setMagFilterMode(FilterMode mode)
		{
			params.magFilterMode = static_cast<rendering::FilterMode>(mode);
			m_sampDesc.Filter = static_cast<D3D11_FILTER>(mode);
		}

		void loadData(unsigned char* textureData)
		{
			ZoneScopedN("[DX11 Texture] loadData()");
			ZeroMemory(&m_sampDesc, sizeof(m_sampDesc));
			setMinFilterMode(static_cast<FilterMode>(params.minFilterMode));
			setWrapMode(0, static_cast<WrapMode>(params.wrapModeR));
			setWrapMode(1, static_cast<WrapMode>(params.wrapModeS));
			setWrapMode(2, static_cast<WrapMode>(params.wrapModeT));
			m_sampDesc.MinLOD = 0;
			m_sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			CHECKERROR(m_windowHandle->dev->CreateSamplerState(&m_sampDesc, &m_texSamplerState), "Texture sampler failed creation", m_windowHandle->checkError());

			ZeroMemory(&m_texDesc, sizeof(m_texDesc));
			m_texDesc.Width = resolution.x;
			m_texDesc.Height = resolution.y;
			setMipCount(params.mipLevels);
			m_texDesc.ArraySize = 1;
			m_texDesc.Format = static_cast<DXGI_FORMAT>(params.format);
			m_texDesc.SampleDesc.Count = 1;
			m_texDesc.SampleDesc.Quality = 0;
			m_texDesc.Usage = m_usageType;
			m_texDesc.BindFlags = m_texType;


			if (textureData != nullptr)
			{
				D3D11_SUBRESOURCE_DATA subData;
				subData.pSysMem = textureData;
				subData.SysMemPitch = m_texDesc.Width * channels;

				CHECKERROR(m_windowHandle->dev->CreateTexture2D(&m_texDesc, &subData, &m_texture), "Texture creation Failed", m_windowHandle->checkError());
			}
			else
			{
				CHECKERROR(m_windowHandle->dev->CreateTexture2D(&m_texDesc, NULL, &m_texture), "Texture creation Failed", m_windowHandle->checkError());
			}

			if (m_texType == D3D11_BIND_SHADER_RESOURCE)
				CHECKERROR(m_windowHandle->dev->CreateShaderResourceView(m_texture, nullptr, &m_shaderResource), "Failed to create the ShaderResourceView", m_windowHandle->checkError());
		}
	};
}
