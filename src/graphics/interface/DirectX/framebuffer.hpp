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
		bool m_draw = false;
		bool m_read = false;
		int m_renderTargetId;
		std::unordered_map<AttachmentSlot, int> m_attachments;
		std::vector<D3D11_RENDER_TARGET_VIEW_DESC> m_renderTargetDescs;
		std::vector<ID3D11RenderTargetView*> m_renderTargetViews;
		std::vector<ID3D11ShaderResourceView*> m_shaderResources;
		std::vector<ID3D11SamplerState*> m_samplerStates;
		std::vector<texture_handle> m_renderTextures;

		ID3D11DepthStencilView* m_depthStencilView = nullptr;
		texture_handle m_depthTexture = nullptr;

		ID3D11RenderTargetView* m_defaultView = nullptr;
		ID3D11DepthStencilView* m_defaultDepthStencilView = nullptr;
		ID3D11Texture2D* m_backBuffer = nullptr;
		ID3D11ShaderResourceView* m_shaderResource = nullptr;
		ID3D11SamplerState* m_backBufferSamplerState = nullptr;
	public:
		void initialize()
		{
			WindowProvider::activeWindow->devcon->OMGetRenderTargets(1, &m_defaultView, &m_defaultDepthStencilView);
			WindowProvider::activeWindow->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_backBuffer);
			WindowProvider::activeWindow->devcon->PSGetShaderResources(0, 1, &m_shaderResource);
			WindowProvider::activeWindow->devcon->PSGetSamplers(0, 1, &m_backBufferSamplerState);
		}

		void bind()
		{
			WindowProvider::activeWindow->devcon->OMSetRenderTargets(m_renderTargetViews.size(), m_renderTargetViews.data(), m_depthStencilView);
			WindowProvider::activeWindow->checkError();
		}

		void unbind()
		{
			WindowProvider::activeWindow->devcon->OMSetRenderTargets(1, &m_defaultView, m_defaultDepthStencilView);
			WindowProvider::activeWindow->checkError();
		}

		void attach(AttachmentSlot attachment, texture_handle handle, bool draw, bool read)
		{
			m_draw = draw;
			m_read = read;

			if (attachment == AttachmentSlot::DEPTH_STENCIL)
			{
				if (m_depthStencilView != nullptr)
					m_depthStencilView->Release();

				m_depthTexture = handle;
				m_depthStencilView = handle->m_impl.m_depthStencilView;
			}
			else
			{
				D3D11_RENDER_TARGET_VIEW_DESC desc;
				desc.Format = handle->m_impl.m_texDesc.Format;
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice = 0;
				if (!m_attachments.count(attachment))
				{
					m_attachments.emplace(attachment, m_renderTargetId);
					m_renderTargetId++;
					m_renderTextures.push_back(handle);
					m_renderTargetViews.emplace_back(nullptr);
					m_shaderResources.emplace_back(handle->m_impl.m_shaderResource);
					m_samplerStates.emplace_back(handle->m_impl.m_texSamplerState);
				}
				else
				{
					m_renderTextures[m_attachments[attachment]] = handle;
					m_shaderResources[m_attachments[attachment]] = handle->m_impl.m_shaderResource;
					m_samplerStates[m_attachments[attachment]] = handle->m_impl.m_texSamplerState;
				}


				//if (m_renderTargetViews.size() > 0 && m_renderTargetViews[m_attachments[attachment]] != nullptr)
				//{
				//	m_renderTargetViews[m_attachments[attachment]]->Release();
				//	m_renderTargetViews[m_attachments[attachment]] = nullptr;
				//}
				if (m_shaderResources.size() > 0 && m_shaderResources[m_attachments[attachment]] != nullptr)
				{
					m_shaderResources[m_attachments[attachment]]->Release();
					m_shaderResources[m_attachments[attachment]] = nullptr;
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
				return m_renderTextures[m_attachments[attachment]];
			if (attachment == AttachmentSlot::DEPTH_STENCIL)
				return m_depthTexture;

			log::error("Attachment \"{}\", was not attached");
			return { nullptr };
		}

		void verify() const
		{

		}

		void release() const
		{
			WindowProvider::activeWindow->checkError();
			ID3D11RenderTargetView* nullRenderTargets[1] = { nullptr };
			WindowProvider::activeWindow->devcon->OMSetRenderTargets(1, nullRenderTargets, nullptr);
			WindowProvider::activeWindow->checkError();
		}

		void rescale(int width, int height)
		{
			if (m_depthTexture != nullptr)
				m_depthTexture->resize(width, height);
			for (auto& [attachment, id] : m_attachments)
			{
				if (m_renderTextures[id] != nullptr)
					m_renderTextures[id]->resize(width, height);

				if (attachment == AttachmentSlot::DEPTH_STENCIL)
					attach(attachment, m_depthTexture, m_draw, m_read);
				else
					attach(attachment, m_renderTextures[id], m_draw, m_read);
			}
		}
	};
}