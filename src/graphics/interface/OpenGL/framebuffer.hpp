#pragma once
#include "graphics/interface/OpenGL/oglincludes.hpp"
#include "graphics/interface/OpenGL/texture.hpp"
#include "graphics/interface/OpenGL/enumtypes.hpp"
#include "graphics/data/texturehandle.hpp"

namespace rythe::rendering::internal
{
	struct framebuffer
	{
	private:
		unsigned int m_id = 0;
		bool m_draw = false;
		bool m_read = false;
		std::unordered_map<AttachmentSlot, texture_handle> m_attachments;
	public:

		void initialize()
		{
			glGenFramebuffers(1, &m_id);
		}

		void bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		}

		void unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void attach(AttachmentSlot attachment, texture_handle texture, bool draw, bool read)
		{
			m_draw = draw;
			m_read = read;
			glFramebufferTexture(GL_FRAMEBUFFER, static_cast<GLenum>(attachment), texture->getId(), 0);

			if (attachment != AttachmentSlot::DEPTH_STENCIL)
			{
				glDrawBuffer(draw ? static_cast<GLenum>(attachment) : GL_NONE);
				glReadBuffer(read ? static_cast<GLenum>(attachment) : GL_NONE);
			}
			m_attachments[attachment] = texture;
		}

		void detach(AttachmentSlot attachment)
		{
			glFramebufferTexture(GL_FRAMEBUFFER, static_cast<GLenum>(attachment), 0, 0);
			m_attachments.erase(attachment);
		}

		texture_handle getAttachment(AttachmentSlot attachment)
		{
			if (m_attachments.count(attachment))
			{
				return m_attachments[attachment];
			}

			log::error("Attachment \"{}\", was not attached", static_cast<int>(attachment));
			return { nullptr };
		}

		bool verify() const
		{
			bool status = glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE;
			if (status)
				log::error("Framebuffer is not complete!");
			else
				log::info("Framebuffer complete");
			return !status;
		}

		void release()
		{
			for (auto& [attachment, handle] : m_attachments)
			{
				detach(attachment);
			}
			unbind();
		}

		void rescale(int width, int height)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);
			//Renderer::RI->setViewport(1, 0, 0, width, height);
			for (auto& [attachment, handle] : m_attachments)
			{
				handle->resize(width, height);
				attach(attachment, handle, m_draw, m_read);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	};
}