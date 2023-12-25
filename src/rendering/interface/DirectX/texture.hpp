#pragma once
#include <string>

#include "rendering/data/textureparameters.hpp"
#include "rendering/cache/windowprovider.hpp"
#include "rendering/interface/DirectX/dx11includes.hpp"
#include "rendering/interface/definitions/window.hpp"

#include <stb/stb_image.h>
#define STB_IMAGE_IMPLEMENTATION

namespace rythe::rendering::internal
{
	struct texture
	{
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
			m_windowHandle = WindowProvider::get(0);
			params = _params;
			m_texType = static_cast<D3D11_BIND_FLAG>(texType);
			m_usageType = static_cast<D3D11_USAGE>(_params.usage);
		}

		void bind(TextureSlot slot)
		{
			m_windowHandle->devcon->PSSetShaderResources(static_cast<UINT>(slot), 1, &m_shaderResource);
			m_windowHandle->devcon->PSSetSamplers(static_cast<UINT>(slot), 1, &m_texSamplerState);
		}

		void loadData(const std::string& filepath, bool flipVertical = true)
		{
			ZeroMemory(&m_sampDesc, sizeof(m_sampDesc));
			m_sampDesc.Filter = static_cast<D3D11_FILTER>(params.filterMode);
			m_sampDesc.AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(params.wrapModeS);
			m_sampDesc.AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(params.wrapModeT);
			m_sampDesc.AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(params.wrapModeT);
			m_sampDesc.MinLOD = 0;
			m_sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			CHECKERROR(m_windowHandle->dev->CreateSamplerState(&m_sampDesc, &m_texSamplerState),"Texture sampler failed creation", m_windowHandle->checkError());

			stbi_set_flip_vertically_on_load(flipVertical);
			data = stbi_load(filepath.c_str(), &resolution.x, &resolution.y, &channels, 0);
			if (!data)
				log::error("Image failed to load");


			ZeroMemory(&m_texDesc, sizeof(m_texDesc));
			m_texDesc.Width = resolution.x;
			m_texDesc.Height = resolution.y;
			m_texDesc.MipLevels = params.mipLevels;
			m_texDesc.ArraySize = 1;
			m_texDesc.Format = static_cast<DXGI_FORMAT>(params.format);
			m_texDesc.SampleDesc.Count = 1;
			m_texDesc.SampleDesc.Quality = 0;
			m_texDesc.Usage = m_usageType;
			m_texDesc.BindFlags = m_texType;

			D3D11_SUBRESOURCE_DATA subData;
			subData.pSysMem = data;
			subData.SysMemPitch = m_texDesc.Width * channels;

			CHECKERROR(m_windowHandle->dev->CreateTexture2D(&m_texDesc, &subData, &m_texture), "Texture creation Failed", m_windowHandle->checkError());
			CHECKERROR(m_windowHandle->dev->CreateShaderResourceView(m_texture, nullptr, &m_shaderResource),"Failed to create the ShaderResourceView", m_windowHandle->checkError());
		}
	};
}
