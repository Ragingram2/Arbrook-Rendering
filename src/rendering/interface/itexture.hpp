#pragma once
#include <string>

#include "rendering/data/textureparameters.hpp"
#include "rendering/interface/definitions/enumtypes.hpp"

namespace rythe::rendering
{
	class TextureCache;
	template<typename APIType>
	struct Itexture;
	namespace internal
	{
		struct texture;
	}
}

namespace rythe::rendering
{
	template<typename APIType>
	struct Itexture
	{
		friend class TextureCache;
		friend struct Itexture<internal::texture>;
	private:
		APIType m_impl;
	public:
		void bind(TextureSlot textureSlot = TextureSlot::TEXTURE0) { m_impl.bind(static_cast<internal::TextureSlot>(textureSlot)); }
		void loadData(const std::string& filepath) { m_impl.loadData(filepath); }

		unsigned int getId() { return m_impl.id; }
		const std::string& getName() { return m_impl.name; }

	private:
		APIType& getImpl() { return m_impl; }
	};
}