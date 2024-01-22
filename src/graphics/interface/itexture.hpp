#pragma once
#include <string>

#include "graphics/data/textureparameters.hpp"
#include "graphics/interface/definitions/enumtypes.hpp"

namespace rythe::rendering
{
	class TextureCache;
	template<typename APIType>
	struct Itexture;

	template<typename APIType>
	struct Iframebuffer;

	namespace internal
	{
		struct texture;
		struct framebuffer;
	}
}

namespace rythe::rendering
{
	template<typename APIType>
	struct Itexture
	{
		friend class TextureCache;
		friend class TextureImporter;
		friend struct Itexture<internal::texture>;
		friend struct internal::framebuffer;
		friend struct Iframebuffer<internal::framebuffer>;
	private:
		APIType m_impl;
	public:
		void bind(TextureSlot textureSlot = TextureSlot::TEXTURE0) { m_impl.bind(static_cast<internal::TextureSlot>(textureSlot)); }
		void unbind() { m_impl.unbind(); }
		void loadData(unsigned char* textureData) { m_impl.loadData(textureData); }

		void setMipCount(int mipCount) { m_impl.setMipCount(mipCount); }
		void setWrapMode(int axis, WrapMode mode) { m_impl.setWrapMode(axis, static_cast<internal::WrapMode>(mode)); }
		void setMinFilterMode(FilterMode mode) { m_impl.setMinFilterMode(static_cast<internal::FilterMode>(mode)); }
		void setMagFilterMode(FilterMode mode) { m_impl.setMagFilterMode(static_cast<internal::FilterMode>(mode)); }

		unsigned int getId() { return m_impl.id; }
		const std::string& getName() { return m_impl.name; }

	private:
		APIType& getImpl() { return m_impl; }
	};
}