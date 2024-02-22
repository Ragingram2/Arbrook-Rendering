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
			bind();
			glFramebufferTexture(GL_FRAMEBUFFER, static_cast<GLenum>(attachment), texture->getId(), 0);

			if (attachment != AttachmentSlot::DEPTH_STENCIL)
			{
				glDrawBuffer(draw ? static_cast<GLenum>(attachment) : GL_NONE);
				glReadBuffer(read ? static_cast<GLenum>(attachment) : GL_NONE);
			}
			unbind();
			m_attachments[attachment] = texture;
		}

		void detach(AttachmentSlot attachment)
		{
			bind();
			glFramebufferTexture(GL_FRAMEBUFFER, static_cast<GLenum>(attachment), 0, 0);
			unbind();
			m_attachments.erase(attachment);
		}

		texture_handle getAttachment(AttachmentSlot attachment)
		{
			if (m_attachments.count(attachment))
			{
				return m_attachments[attachment];
			}

			log::error("Attachment \"{}\", was not attached");
			return { nullptr };
		}

		void verify() const
		{
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				log::error("Framebuffer is not complete!");
			else
				log::info("Framebuffer complete");
		}

		void release()
		{
			for (auto& [attachment, handle] : m_attachments)
			{
				detach(attachment);
			}
			unbind();
		}
	};
}