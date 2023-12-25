#pragma once
#include <string>


#include <rsl/logging>

#include "rendering/data/textureparameters.hpp"
#include "rendering/interface/OpenGL/oglincludes.hpp"

#include <stb/stb_image.h>
#define STB_IMAGE_IMPLEMENTATION

namespace rythe::rendering::internal
{
	namespace log = rsl::log;
	struct texture
	{
	private:
		GLenum m_texType;
		GLenum m_usageType;
		int mipCount = 0;
	public:
		int channels;
		math::ivec2 resolution;
		unsigned int id;
		std::string name;
		unsigned char* data;
		texture_parameters params;
		texture() = default;
		texture(unsigned int id, std::string name) : id(id), name(name) { }
		texture(texture* other)
		{
			name = other->name;
			id = other->id;
			data = other->data;
			params = other->params;
		}

		void initialize(TargetType _texType, texture_parameters _params)
		{
			params = _params;
			m_texType = static_cast<GLenum>(_texType);
			m_usageType = static_cast<GLenum>(params.usage);

			glGenTextures(1, &id);
			bind();
			glTexParameteri(m_texType, GL_TEXTURE_WRAP_R, static_cast<GLint>(params.wrapModeR));
			glTexParameteri(m_texType, GL_TEXTURE_WRAP_S, static_cast<GLint>(params.wrapModeS));
			glTexParameteri(m_texType, GL_TEXTURE_WRAP_T, static_cast<GLint>(params.wrapModeT));
			glTexParameteri(m_texType, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(params.filterMode));
			glTexParameteri(m_texType, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(params.filterMode));

			mipCount = params.generateMipMaps ? params.mipLevels : 1;
			glTexParameteri(m_texType, GL_TEXTURE_MAX_LEVEL, mipCount);
		}

		void bind(TextureSlot textureSlot = TextureSlot::TEXTURE0)
		{
			glActiveTexture(static_cast<GLenum>(textureSlot));
			glBindTexture(m_texType, id);
		}

		void loadData(const std::string& filepath, bool flipVertical = true)
		{
			stbi_set_flip_vertically_on_load(flipVertical);
			data = stbi_load(filepath.c_str(), &resolution.x, &resolution.y, &channels, 0);
			if (!data)
				log::error("Image failed to load");

			log::debug("Channel Count: {}", channels);
			switch (m_usageType)
			{
			case static_cast<GLenum>(0):

				glTexStorage2D(m_texType, static_cast<GLint>(mipCount), static_cast<GLint>(params.format), resolution.x, resolution.y);
				if (channels == 4)
					glTexSubImage2D(m_texType, 0, 0, 0, resolution.x, resolution.y, GL_RGBA, GL_UNSIGNED_BYTE, data);
				else if( channels == 3)
					glTexSubImage2D(m_texType, 0, 0, 0, resolution.x, resolution.y, GL_RGB, GL_UNSIGNED_BYTE, data);
				else if(channels == 2)
					glTexSubImage2D(m_texType, 0, 0, 0, resolution.x, resolution.y, GL_RG, GL_UNSIGNED_BYTE, data);
				else if(channels == 1)
					glTexSubImage2D(m_texType, 0, 0, 0, resolution.x, resolution.y, GL_RED, GL_UNSIGNED_BYTE, data);
				break;
			case GL_DYNAMIC_DRAW:
				if (channels == 4)
					glTexImage2D(m_texType, 0, static_cast<GLint>(params.format), resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				else if (channels == 3)
					glTexImage2D(m_texType, 0, static_cast<GLint>(params.format), resolution.x, resolution.y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				else if (channels == 2)
					glTexImage2D(m_texType, 0, static_cast<GLint>(params.format), resolution.x, resolution.y, 0, GL_RG, GL_UNSIGNED_BYTE, data);
				else if (channels == 1)
					glTexImage2D(m_texType, 0, static_cast<GLint>(params.format), resolution.x, resolution.y, 0, GL_RED, GL_UNSIGNED_BYTE, data);
				break;
			default:
				break;
			}

			if (params.generateMipMaps)
			{
				glGenerateMipmap(m_texType);
			}
		}
	};
}