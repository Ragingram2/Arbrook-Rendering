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
		std::vector<DXRenderTargetView> m_renderTargetViews;
		std::vector<DXSamplerState> m_samplerStates;
		std::vector<texture_handle> m_renderTextures;

		texture_handle m_depthTexture = nullptr;
	public:
		void initialize()
		{
		}

		void bind()
		{
			if (m_depthTexture != nullptr)
			{
				WindowProvider::activeWindow->devcon->OMSetRenderTargets(m_renderTargetViews.size(), m_renderTargetViews.data()->GetAddressOf(), m_depthTexture->m_impl.m_depthStencilView.Get());
			}
			else 
			{
				WindowProvider::activeWindow->devcon->OMSetRenderTargets(m_renderTargetViews.size(), m_renderTargetViews.data()->GetAddressOf(), NULL);
			}
			WindowProvider::activeWindow->checkError();
		}

		void unbind()
		{

			//if (m_depthTexture != nullptr)
			//{
			//	m_depthTexture->unbind(rendering::TextureSlot::TEXTURE0);
			//}

			//for (auto& [attachment, id] : m_attachments)
			//{
			//	if (m_renderTextures[id] != nullptr)
			//	{
			//		m_renderTextures[id]->unbind(rendering::TextureSlot::TEXTURE0);
			//	}
			//}

			WindowProvider::activeWindow->devcon->OMSetRenderTargets(1, WindowProvider::activeWindow->renderTargetView.GetAddressOf(), WindowProvider::activeWindow->depthStencilView.Get());
			WindowProvider::activeWindow->checkError();
		}

		void attach(AttachmentSlot attachment, texture_handle handle, bool draw, bool read)
		{
			m_draw = draw;
			m_read = read;

			if (attachment == AttachmentSlot::DEPTH_STENCIL)
			{
				m_depthTexture = handle;
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
				}
				else
				{
					m_renderTextures[m_attachments[attachment]] = handle;
				}

				CHECKERROR(WindowProvider::activeWindow->dev->CreateRenderTargetView(handle->m_impl.m_texture.Get(), &desc, m_renderTargetViews[m_attachments[attachment]].GetAddressOf()), "Render Target View", WindowProvider::activeWindow->checkError());

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

			log::error("Attachment \"{}\", was not attached",static_cast<int>(attachment));
			return { nullptr };
		}

		void verify() const
		{

		}

		void release()
		{
			for (auto& view : m_renderTargetViews)
			{
				if (view == nullptr) continue;

				view.Reset();
			}

			if (m_depthTexture != nullptr)
			{
				m_depthTexture->release();
			}

			for (auto& [attachment, id] : m_attachments)
			{
				if (m_renderTextures[id] != nullptr)
				{
					m_renderTextures[id]->release();
				}
			}

			WindowProvider::activeWindow->devcon->ClearState();
			WindowProvider::activeWindow->devcon->Flush();
		}

		void rescale(int width, int height)
		{
			//release();

			if (m_depthTexture != nullptr)
			{
				m_depthTexture->resize(width, height);

				attach(AttachmentSlot::DEPTH_STENCIL, m_depthTexture, m_draw, m_read);
			}

			for (auto& [attachment, id] : m_attachments)
			{
				if (m_renderTextures[id] != nullptr)
				{
					m_renderTextures[id]->resize(width, height);
				}

				if (attachment != AttachmentSlot::DEPTH_STENCIL)
					attach(attachment, m_renderTextures[id], m_draw, m_read);
			}
		}
	};
}