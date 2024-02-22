#pragma once
#include <string>

#include "core/utils/profiler.hpp"

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
		ID3D11DepthStencilView* m_depthStencilView = nullptr;
		ID3D11ShaderResourceView* m_shaderResource = nullptr;
		ID3D11SamplerState* m_texSamplerState = nullptr;
		D3D11_SAMPLER_DESC m_sampDesc;
		D3D11_TEXTURE2D_DESC m_texDesc;
		D3D11_BIND_FLAG m_texType;
		D3D11_USAGE m_usageType;
		TargetType m_type;
	public:
		int channels;
		math::ivec2 resolution;
		unsigned int id;
		std::string name;
		unsigned char* data;
		texture_parameters params;
		TextureSlot activeSlot;
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
			params = _params;
			m_type = texType;
			m_texType = static_cast<D3D11_BIND_FLAG>(texType);
			m_usageType = static_cast<D3D11_USAGE>(_params.usage);
		}

		void bind(TextureSlot slot)
		{
			activeSlot = slot;
			ZoneScopedN("[DX11 Texture] bind()");
			//log::debug("Binding Texture \"{}\"", name);
			if (m_shaderResource == nullptr)
			{
				log::warn("Shader Resource is null, this is ok if this was intended, but this has the same effect as unbinding a texture here");
			}

			WindowProvider::activeWindow->devcon->VSSetShaderResources(static_cast<UINT>(slot), 1, &m_shaderResource);
			WindowProvider::activeWindow->devcon->VSSetSamplers(static_cast<UINT>(slot), 1, &m_texSamplerState);

			WindowProvider::activeWindow->devcon->GSSetShaderResources(static_cast<UINT>(slot), 1, &m_shaderResource);
			WindowProvider::activeWindow->devcon->GSSetSamplers(static_cast<UINT>(slot), 1, &m_texSamplerState);

			WindowProvider::activeWindow->devcon->PSSetShaderResources(static_cast<UINT>(slot), 1, &m_shaderResource);
			WindowProvider::activeWindow->devcon->PSSetSamplers(static_cast<UINT>(slot), 1, &m_texSamplerState);
		}

		void unbind(TextureSlot slot)
		{
			ZoneScopedN("[DirectX Texture] unbind()");

			//log::debug("Unbinding Texture \"{}\"", name);

			ID3D11ShaderResourceView* nullResource[1] = { nullptr };
			ID3D11SamplerState* nullSampler[1] = { nullptr };

			WindowProvider::activeWindow->devcon->VSSetShaderResources(static_cast<UINT>(slot), 1, nullResource);
			WindowProvider::activeWindow->devcon->VSSetSamplers(static_cast<UINT>(slot), 1, nullSampler);

			WindowProvider::activeWindow->devcon->GSSetShaderResources(static_cast<UINT>(slot), 1, nullResource);
			WindowProvider::activeWindow->devcon->GSSetSamplers(static_cast<UINT>(slot), 1, nullSampler);

			WindowProvider::activeWindow->devcon->PSSetShaderResources(static_cast<UINT>(slot), 1, nullResource);
			WindowProvider::activeWindow->devcon->PSSetSamplers(static_cast<UINT>(slot), 1, nullSampler);
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

			CHECKERROR(WindowProvider::activeWindow->dev->CreateSamplerState(&m_sampDesc, &m_texSamplerState), "Texture sampler failed creation", WindowProvider::activeWindow->checkError());

			ZeroMemory(&m_texDesc, sizeof(m_texDesc));
			m_texDesc.Width = resolution.x;
			m_texDesc.Height = resolution.y;
			setMipCount(params.mipLevels);
			m_texDesc.ArraySize = m_type == TargetType::CUBEMAP ? 6 : params.textures;
			m_texDesc.Format = static_cast<DXGI_FORMAT>(params.format);
			m_texDesc.SampleDesc.Count = 1;
			m_texDesc.SampleDesc.Quality = 0;
			m_texDesc.Usage = m_usageType;
			if (m_type == TargetType::CUBEMAP)
				m_texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

			if (m_texType == D3D11_BIND_RENDER_TARGET)
				m_texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			else if (m_texType == D3D11_BIND_DEPTH_STENCIL)
				m_texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
			else if (m_texType == D3D11_BIND_SHADER_RESOURCE + 1)
				m_texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			else
				m_texDesc.BindFlags = m_texType;

			if (textureData != nullptr)
			{
				//if (m_texDesc.MiscFlags == D3D11_RESOURCE_MISC_TEXTURECUBE)
				//{
				//	D3D11_SUBRESOURCE_DATA cubeData[params.textures];
				//	for (int i = 0; i < params.textures; i++)
				//	{
				//		cubeData[i].pSysMem = textureData;
				//		cubeData[i].SysMemPitch = m_texDesc.Width * channels;
				//	}
				//	CHECKERROR(WindowProvider::activeWindow->dev->CreateTexture2D(&m_texDesc, &cubeData[0], &m_texture), "Texture creation Failed", WindowProvider::activeWindow->checkError());
				//}
				//else
				//{
				D3D11_SUBRESOURCE_DATA subData;
				subData.pSysMem = textureData;
				subData.SysMemPitch = m_texDesc.Width * channels;

				CHECKERROR(WindowProvider::activeWindow->dev->CreateTexture2D(&m_texDesc, &subData, &m_texture), "Texture creation Failed", WindowProvider::activeWindow->checkError());
			}
			else
			{
				CHECKERROR(WindowProvider::activeWindow->dev->CreateTexture2D(&m_texDesc, NULL, &m_texture), "Texture creation Failed", WindowProvider::activeWindow->checkError());
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
			shaderResourceViewDesc.Format = m_texDesc.Format;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = m_texDesc.MipLevels;

			if (m_texType == D3D11_BIND_SHADER_RESOURCE || m_texType == D3D11_BIND_RENDER_TARGET)
			{
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				CHECKERROR(WindowProvider::activeWindow->dev->CreateShaderResourceView(m_texture, &shaderResourceViewDesc, &m_shaderResource), "Failed to create the Shader Resource View", WindowProvider::activeWindow->checkError());
			}
			else if (m_texType == D3D11_BIND_SHADER_RESOURCE + 1 && m_texDesc.MiscFlags == D3D11_RESOURCE_MISC_TEXTURECUBE)
			{
				shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				CHECKERROR(WindowProvider::activeWindow->dev->CreateShaderResourceView(m_texture, &shaderResourceViewDesc, &m_shaderResource), "Failed to create the Shader Resource View", WindowProvider::activeWindow->checkError());
			}
			else if (m_texType == D3D11_BIND_DEPTH_STENCIL)
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
				ZeroMemory(&m_depthStencilViewDesc, sizeof(m_depthStencilViewDesc));
				m_depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				m_depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				CHECKERROR(WindowProvider::activeWindow->dev->CreateDepthStencilView(m_texture, &m_depthStencilViewDesc, &m_depthStencilView), "Creating Depth Stencil View failed", WindowProvider::activeWindow->checkError());

				shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				CHECKERROR(WindowProvider::activeWindow->dev->CreateShaderResourceView(m_texture, &shaderResourceViewDesc, &m_shaderResource), "Failed to create the Shader Resource View", WindowProvider::activeWindow->checkError());
			}
		}
	};
}
