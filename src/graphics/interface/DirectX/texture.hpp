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
	public:
		int channels;
		math::ivec2 resolution;
		unsigned int id;
		std::string name;
		unsigned char* data;
		texture_parameters params;
		rendering::TextureSlot activeSlot;
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
			m_texType = static_cast<D3D11_BIND_FLAG>(texType);
			m_usageType = static_cast<D3D11_USAGE>(_params.usage);
		}

		void bind(TextureSlot slot)
		{
			ZoneScopedN("[DX11 Texture] bind()");
			//log::debug("Binding texture \"{}\"", name);
			WindowProvider::activeWindow->checkError();
			WindowProvider::activeWindow->devcon->PSSetShaderResources(static_cast<UINT>(slot), 1, &m_shaderResource);
			WindowProvider::activeWindow->checkError();
			WindowProvider::activeWindow->devcon->PSSetSamplers(static_cast<UINT>(slot), 1, &m_texSamplerState);
			WindowProvider::activeWindow->checkError();
		}

		void unbind(TextureSlot slot)
		{
			ZoneScopedN("[DirectX Texture] unbind()");
			//log::debug("Unbinding texture \"{}\"", name);
			ID3D11ShaderResourceView* nullResource[1] = { nullptr };
			WindowProvider::activeWindow->devcon->PSSetShaderResources(static_cast<UINT>(slot), 1, nullResource);
			WindowProvider::activeWindow->checkError();
			ID3D11SamplerState* nullSampler[1] = { nullptr };
			WindowProvider::activeWindow->devcon->PSSetSamplers(static_cast<UINT>(slot), 1, nullSampler);
			WindowProvider::activeWindow->checkError();
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
			m_texDesc.ArraySize = 1;
			m_texDesc.Format = static_cast<DXGI_FORMAT>(params.format);
			m_texDesc.SampleDesc.Count = 1;
			m_texDesc.SampleDesc.Quality = 0;
			m_texDesc.Usage = m_usageType;
			if (m_texType == D3D11_BIND_RENDER_TARGET)
				m_texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			else
				m_texDesc.BindFlags = m_texType;

			if (textureData != nullptr)
			{
				D3D11_SUBRESOURCE_DATA subData;
				subData.pSysMem = textureData;
				subData.SysMemPitch = m_texDesc.Width * channels;

				CHECKERROR(WindowProvider::activeWindow->dev->CreateTexture2D(&m_texDesc, &subData, &m_texture), "Texture creation Failed", WindowProvider::activeWindow->checkError());
			}
			else
			{
				CHECKERROR(WindowProvider::activeWindow->dev->CreateTexture2D(&m_texDesc, NULL, &m_texture), "Texture creation Failed", WindowProvider::activeWindow->checkError());
			}

			if (m_texType == D3D11_BIND_SHADER_RESOURCE || m_texType == D3D11_BIND_RENDER_TARGET)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
				ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
				shaderResourceViewDesc.Format = m_texDesc.Format;
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
				shaderResourceViewDesc.Texture2D.MipLevels = m_texDesc.MipLevels;
				CHECKERROR(WindowProvider::activeWindow->dev->CreateShaderResourceView(m_texture, &shaderResourceViewDesc, &m_shaderResource), "Failed to create the Shader Resource View", WindowProvider::activeWindow->checkError());
			}
			else if (m_texType == D3D11_BIND_DEPTH_STENCIL)
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
				ZeroMemory(&m_depthStencilViewDesc, sizeof(m_depthStencilViewDesc));
				m_depthStencilViewDesc.Format = m_texDesc.Format;
				m_depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				CHECKERROR(WindowProvider::activeWindow->dev->CreateDepthStencilView(m_texture, &m_depthStencilViewDesc, &m_depthStencilView), "Creating Depth Stencil View failed", WindowProvider::activeWindow->checkError());
			}
		}
	};
}
