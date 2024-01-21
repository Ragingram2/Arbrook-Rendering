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
		void bind() const { m_impl.bind(); }
		void unbind() const { m_impl.unbind(); }
		void attach(AttachmentSlot attachment, texture_handle texture) { m_impl.attach(static_cast<internal::AttachmentSlot>(attachment), texture); }
		void detach(AttachmentSlot attachment) { m_impl.detach(static_cast<internal::AttachmentSlot>(attachment)); }
		texture_handle getAttachment(AttachmentSlot attachment) { return m_impl.getAttachment(static_cast<internal::AttachmentSlot>(attachment)); }
		void verify() { m_impl.verify(); }
		void release() { m_impl.release(); }

	private:

		APIType& getImpl() { return m_impl; }
	};
}