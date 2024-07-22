#pragma once
#include <string>

#include "core/utils/profiler.hpp"

#include "graphics/data/textureparameters.hpp"
#include "graphics/cache/windowprovider.hpp"
#include "graphics/interface/DirectX/dx11includes.hpp"
#include "graphics/interface/definitions/window.hpp"

namespace rythe::rendering::internal
{
	class RenderInterface;
	struct framebuffer;
	struct texture
	{
		friend struct framebuffer;
		friend class RenderInterface;
	private:
		unsigned char* m_textureData;
		DXTexture2D m_texture;
		DXDepthStencilView m_depthStencilView = nullptr;
		DXShaderResourceView m_shaderResource = nullptr;
		DXSamplerState m_texSamplerState = nullptr;

		D3D11_SAMPLER_DESC m_sampDesc;
		D3D11_TEXTURE2D_DESC m_texDesc;
		D3D11_BIND_FLAG m_texType;
		D3D11_USAGE m_usageType;
		TextureType m_type;
	public:
		int channels;
		math::ivec2 resolution;
		unsigned int id;
		void* internalHandle;
		std::string name;
		unsigned char* data;
		texture_parameters params;
		TextureSlot slot;
		texture() = default;
		texture(unsigned int id, std::string name) : id(id), name(name) { }
		texture(texture* other)
		{
			name = other->name;
			id = other->id;
			data = other->data;
			params = other->params;
		}

		void initialize(TextureType texType, texture_parameters _params)
		{
			ZoneScopedN("[DX11 Texture] initialize()");
			params = _params;
			m_type = texType;
			m_texType = static_cast<D3D11_BIND_FLAG>(texType);
			m_usageType = static_cast<D3D11_USAGE>(_params.usage);
		}

		void bind(TextureSlot slot)
		{
			this->slot = slot;
			ZoneScopedN("[DX11 Texture] bind()");
			if (m_shaderResource == nullptr)
			{
				log::warn("Shader Resource is null, this is ok if this was intended, but this has the same effect as unbinding a texture here");
			}

		ID3D11ShaderResourceView* resource[1] = { m_shaderResource.Get() };
			ID3D11SamplerState* sampler[1] = { m_texSamplerState.Get() };

			WindowProvider::activeWindow->devcon->VSSetShaderResources(static_cast<UINT>(slot), 1, resource);
			WindowProvider::activeWindow->devcon->VSSetSamplers(static_cast<UINT>(slot), 1, sampler);

			WindowProvider::activeWindow->devcon->GSSetShaderResources(static_cast<UINT>(slot), 1, resource);
			WindowProvider::activeWindow->devcon->GSSetSamplers(static_cast<UINT>(slot), 1, sampler);

			WindowProvider::activeWindow->devcon->PSSetShaderResources(static_cast<UINT>(slot), 1, resource);
			WindowProvider::activeWindow->devcon->PSSetSamplers(static_cast<UINT>(slot), 1, sampler);
		}

		void unbind(TextureSlot slot)
		{
			ZoneScopedN("[DirectX Texture] unbind()");

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


			m_textureData = textureData;
			ZeroMemory(&m_sampDesc, sizeof(m_sampDesc));
			setMinFilterMode(static_cast<FilterMode>(params.minFilterMode));
			setWrapMode(0, static_cast<WrapMode>(params.wrapModeR));
			setWrapMode(1, static_cast<WrapMode>(params.wrapModeS));
			setWrapMode(2, static_cast<WrapMode>(params.wrapModeT));
			m_sampDesc.MinLOD = 0;
			m_sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
			m_sampDesc.BorderColor[0] = params.borderColor.r;
			m_sampDesc.BorderColor[1] = params.borderColor.g;
			m_sampDesc.BorderColor[2] = params.borderColor.b;
			m_sampDesc.BorderColor[3] = params.borderColor.a;

			CHECKERROR(WindowProvider::activeWindow->dev->CreateSamplerState(&m_sampDesc, &m_texSamplerState), "Texture sampler failed creation", WindowProvider::activeWindow->checkError());

			ZeroMemory(&m_texDesc, sizeof(m_texDesc));
			m_texDesc.Width = resolution.x;
			m_texDesc.Height = resolution.y;
			setMipCount(params.mipLevels);
			m_texDesc.ArraySize = m_type == TextureType::CUBEMAP ? 6 : params.textures;
			m_texDesc.Format = static_cast<DXGI_FORMAT>(params.format);
			m_texDesc.SampleDesc.Count = 1;
			m_texDesc.SampleDesc.Quality = 0;
			m_texDesc.Usage = params.usage == rendering::UsageType::DEPTH_COMPONENT ? D3D11_USAGE_DEFAULT : m_usageType;
			if (m_type == TextureType::CUBEMAP)
				m_texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

			if (m_texType == D3D11_BIND_RENDER_TARGET)
				m_texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			else if (m_texType == D3D11_BIND_DEPTH_STENCIL || params.usage == rendering::UsageType::DEPTH_COMPONENT)
				m_texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
			else if (m_texType == D3D11_BIND_SHADER_RESOURCE + 1)
				m_texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
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



			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
			shaderResourceViewDesc.Format = m_texDesc.Format;


			switch (m_type)
			{
			case TextureType::RENDER_TARGET:
			case TextureType::TEXTURE2D:
				shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
				shaderResourceViewDesc.Texture2D.MipLevels = m_texDesc.MipLevels;
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				CHECKERROR(WindowProvider::activeWindow->dev->CreateShaderResourceView(m_texture.Get(), &shaderResourceViewDesc, &m_shaderResource), "Failed to create the Shader Resource View", WindowProvider::activeWindow->checkError());
				break;
			case TextureType::CUBEMAP:
				if (params.usage == rendering::UsageType::DEPTH_COMPONENT)
				{
					D3D11_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
					ZeroMemory(&m_depthStencilViewDesc, sizeof(m_depthStencilViewDesc));
					m_depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
					m_depthStencilViewDesc.Flags = 0;
					m_depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
					m_depthStencilViewDesc.Texture2DArray.MipSlice = 0;
					m_depthStencilViewDesc.Texture2DArray.ArraySize = m_texDesc.ArraySize;
					m_depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
					CHECKERROR(WindowProvider::activeWindow->dev->CreateDepthStencilView(m_texture.Get(), &m_depthStencilViewDesc, &m_depthStencilView), "Creating Depth Stencil View failed", WindowProvider::activeWindow->checkError());

					shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
					shaderResourceViewDesc.Texture2DArray.MipLevels = m_texDesc.MipLevels;
					shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					CHECKERROR(WindowProvider::activeWindow->dev->CreateShaderResourceView(m_texture.Get(), &shaderResourceViewDesc, &m_shaderResource), "Failed to create the Shader Resource View", WindowProvider::activeWindow->checkError());

					//log::debug("[{}] Creating DSV for cubemap {}", name, static_cast<void*>(m_depthStencilView.Get()));
					//log::debug("[{}] Creating SRV for cubemap {}", name, static_cast<void*>(m_shaderResource.Get()));
				}
				else
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					CHECKERROR(WindowProvider::activeWindow->dev->CreateShaderResourceView(m_texture.Get(), &shaderResourceViewDesc, &m_shaderResource), "Failed to create the Shader Resource View", WindowProvider::activeWindow->checkError());
					//log::debug("[{}] Creating SRV for cubemap {}", name, static_cast<void*>(m_shaderResource.Get()));
				}
				break;
			case TextureType::DEPTH_STENCIL:
				D3D11_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
				ZeroMemory(&m_depthStencilViewDesc, sizeof(m_depthStencilViewDesc));
				m_depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				m_depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				CHECKERROR(WindowProvider::activeWindow->dev->CreateDepthStencilView(m_texture.Get(), &m_depthStencilViewDesc, &m_depthStencilView), "Creating Depth Stencil View failed", WindowProvider::activeWindow->checkError());

				shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
				shaderResourceViewDesc.Texture2D.MipLevels = m_texDesc.MipLevels;
				shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				CHECKERROR(WindowProvider::activeWindow->dev->CreateShaderResourceView(m_texture.Get(), &shaderResourceViewDesc, &m_shaderResource), "Failed to create the Shader Resource View", WindowProvider::activeWindow->checkError());

				//log::debug("[{}] Creating DSV for DepthStencil {}", name, static_cast<void*>(m_depthStencilView.Get()));
				//log::debug("[{}] Creating SRV for DepthStencil {}", name, static_cast<void*>(m_shaderResource.Get()));
				break;
			default:
				break;
			}

			internalHandle = static_cast<void*>(m_shaderResource.Get());
		}

		void resize(int width, int height)
		{
			resolution = math::vec2(width, height);

			if (params.textures == 1)
			{
				loadData(m_textureData);
			}
		}

		void release()
		{
			if (m_texture != nullptr)
			{
				//log::debug("[{}] Release Texture \"{}\"", name, static_cast<void*>(m_texture.Get()));
				m_texture.Reset();
			}
			if (m_shaderResource != nullptr)
			{
				//log::debug("[{}] Release SRV \"{}\"", name, static_cast<void*>(m_shaderResource.Get()));
				m_shaderResource.Reset();
			}
			if (m_depthStencilView != nullptr)
			{
				//log::debug("[{}] Release DSV \"{}\"", name, static_cast<void*>(m_depthStencilView.Get()));
				m_depthStencilView.Reset();
			}

			WindowProvider::activeWindow->devcon->ClearState();
			WindowProvider::activeWindow->devcon->Flush();
		}
	};
}
