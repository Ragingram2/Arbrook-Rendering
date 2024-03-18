#pragma once

#include "graphics/data/texturehandle.hpp"
#include "graphics/interface/definitions/enumtypes.hpp"

namespace rythe::rendering
{
	template<typename APIType>
	struct Iframebuffer
	{
	private:
		APIType m_impl;
	public:

		void initialize() { m_impl.initialize(); }
		void bind() { m_impl.bind(); }
		void unbind() { m_impl.unbind(); }
		void attach(AttachmentSlot attachment, texture_handle texture, bool draw = true, bool read = false) { m_impl.attach(static_cast<internal::AttachmentSlot>(attachment), texture, draw, read); }
		void detach(AttachmentSlot attachment) { m_impl.detach(static_cast<internal::AttachmentSlot>(attachment)); }
		texture_handle getAttachment(AttachmentSlot attachment) { return m_impl.getAttachment(static_cast<internal::AttachmentSlot>(attachment)); }
		bool verify() { return m_impl.verify(); }
		void release() { m_impl.release(); }

		void rescale(int width, int height) { m_impl.rescale(width, height); }

	private:

		APIType& getImpl() { return m_impl; }
	};
}