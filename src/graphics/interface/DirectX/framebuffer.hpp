#pragma once
#include "graphics/interface/DirectX/dx11includes.hpp"
#include "graphics/interface/DirectX/texture.hpp"
#include "graphics/interface/DirectX/enumtypes.hpp"
#include "graphics/data/texturehandle.hpp"

namespace rythe::rendering::internal
{

	struct framebuffer
	{
		friend class RenderInterface;
	private:
		int m_renderTargetId;
		std::unordered_map<AttachmentSlot, int> m_attachments;
		std::vector<D3D11_RENDER_TARGET_VIEW_DESC> m_renderTargetDescs;
		std::vector<ID3D11RenderTargetView*> m_renderTargetViews;
		std::vector<texture_handle> m_renderTextures;

		D3D11_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
		ID3D11DepthStencilView* m_depthStencilView;
		texture_handle m_depthTexture;

		ID3D11RenderTargetView* m_defaultView;
	public:
		void initialize()
		{
			//getting the default color attachment
			ID3D11Texture2D* m_backBuffer;
			HRESULT hr = WindowProvider::activeWindow->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_backBuffer);
			CHECKERROR(hr, "Retrieving the backbuffer failed", WindowProvider::activeWindow->checkError());

			//assigning the default color attachment
			hr = WindowProvider::activeWindow->dev->CreateRenderTargetView(m_backBuffer, NULL, &m_defaultView);
			CHECKERROR(hr, "Creating a render target view failed", WindowProvider::activeWindow->checkError());

			m_backBuffer->Release();
		}

		void bind() const
		{
			WindowProvider::activeWindow->devcon->OMSetRenderTargets(m_renderTargetViews.size(), m_renderTargetViews.data(), m_depthStencilView);
		}

		void unbind() const
		{
			WindowProvider::activeWindow->devcon->OMSetRenderTargets(1, &m_defaultView, m_depthStencilView);
		}

		void attach(AttachmentSlot attachment, texture_handle handle)
		{
			if (attachment == AttachmentSlot::DEPTH_STENCIL)
			{
				m_depthTexture = handle;
				m_depthStencilViewDesc.Format = handle->m_impl.m_texDesc.Format;
				m_depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				CHECKERROR(WindowProvider::activeWindow->dev->CreateDepthStencilView(handle->m_impl.m_texture, &m_depthStencilViewDesc, &m_depthStencilView), "Creating Depth Stencil View failed", WindowProvider::activeWindow->checkError());
			}
			else
			{
				D3D11_RENDER_TARGET_VIEW_DESC desc;
				auto pos = std::find(m_renderTextures.begin(), m_renderTextures.end(), handle);
				if (!m_attachments.count(attachment))
				{
					m_attachments.emplace(attachment, m_renderTargetId);
					m_renderTargetId++;
					m_renderTextures.push_back(handle);
					m_renderTargetViews.emplace_back(nullptr);
					desc = m_renderTargetDescs.emplace_back(D3D11_RENDER_TARGET_VIEW_DESC{});
					desc.Format = handle->m_impl.m_texDesc.Format;
					desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				}
				else
				{
					desc = m_renderTargetDescs[m_attachments[attachment]];
				}

				CHECKERROR(WindowProvider::activeWindow->dev->CreateRenderTargetView(handle->m_impl.m_texture, &desc, &m_renderTargetViews[m_attachments[attachment]]), "Render Target View", WindowProvider::activeWindow->checkError());
			}
		}

		void detach(AttachmentSlot attachment)
		{

		}

		texture_handle getAttachment(AttachmentSlot attachment)
		{
			if (m_attachments.count(attachment))
			{
				if (attachment == AttachmentSlot::DEPTH_STENCIL)
					return m_depthTexture;
				else
					return m_renderTextures[m_attachments[attachment]];
			}

			log::error("Attachment \"{}\", was not attached");
			return { nullptr };
		}

		void verify() const
		{

		}

		void release()
		{

		}
	};
}