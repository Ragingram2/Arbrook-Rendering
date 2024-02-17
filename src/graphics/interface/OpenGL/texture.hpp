#pragma once
#include <string>

#include <rsl/logging>

#include "core/utils/profiler.hpp"

#include "graphics/data/textureparameters.hpp"
#include "graphics/interface/OpenGL/oglincludes.hpp"

namespace rythe::rendering::internal
{
	inline std::unordered_map<TextureSlot, std::pair<const char*, int>> textureSlotNames = {
	{TextureSlot::TEXTURE0,std::make_pair("Texture0",0)},
	{TextureSlot::TEXTURE1,std::make_pair("Texture1",1)},
	{TextureSlot::TEXTURE2,std::make_pair("Texture2",2)},
	{TextureSlot::TEXTURE3,std::make_pair("Texture3",3)},
	{TextureSlot::TEXTURE4,std::make_pair("Texture4",4)},
	{TextureSlot::TEXTURE5,std::make_pair("Texture5",5)}
	};

	namespace log = rsl::log;
	struct texture
	{
	private:
		GLenum m_texType;
		GLenum m_usageType;
	public:
		int channels;
		math::ivec2 resolution;
		unsigned int id = 0;
		std::string name;
		texture_parameters params;
		TextureSlot slot = TextureSlot::TEXTURE0;
		texture() = default;
		texture(unsigned int id, std::string name) : id(id), name(name) { }
		texture(texture* other)
		{
			name = other->name;
			id = other->id;
			params = other->params;
		}

		void initialize(TargetType _texType, texture_parameters _params)
		{
			ZoneScopedN("[OpenGL Texture] initialize()");
			params = _params;
			m_texType = static_cast<GLenum>(_texType);
			m_usageType = static_cast<GLenum>(params.usage);

			glGenTextures(1, &id);
			bind(slot);
			glTexParameterfv(m_texType, GL_TEXTURE_BORDER_COLOR, params.borderColor.data);
			setWrapMode(0, static_cast<WrapMode>(params.wrapModeS));
			setWrapMode(1, static_cast<WrapMode>(params.wrapModeT));
			setWrapMode(2, static_cast<WrapMode>(params.wrapModeR));
			setMinFilterMode(static_cast<FilterMode>(params.minFilterMode));
			setMagFilterMode(static_cast<FilterMode>(params.magFilterMode));
			setMipCount(params.mipLevels);
		}

		void bind(TextureSlot textureSlot)
		{
			ZoneScopedN("[OpenGL Texture] bind()");
			slot = textureSlot;
			glActiveTexture(static_cast<GLenum>(textureSlot));
			glBindTexture(m_texType, id);


			GLint shaderId;
			glGetIntegerv(GL_CURRENT_PROGRAM, &shaderId);

			if (shaderId != 0)
			{
				auto pair = textureSlotNames[textureSlot];
				glUniform1i(glGetUniformLocation(shaderId, pair.first), pair.second);
			}
			else
				log::warn("Attempting to bind a texture to a texture slot without a bound shader");
		}

		void unbind(TextureSlot textureSlot)
		{
			ZoneScopedN("[OpenGL Texture] unbind()");
			glBindTexture(m_texType, 0);
		}

		void setMipCount(int mipCount)
		{
			if (!params.generateMipMaps)
			{
				return;
			}
			params.mipLevels = mipCount = mipCount >= 0 ? mipCount : 1;
			glTexParameteri(m_texType, GL_TEXTURE_MAX_LEVEL, mipCount);
		}

		void setWrapMode(int axis, WrapMode mode)
		{
			switch (axis)
			{
			case 0:
				glTexParameteri(m_texType, GL_TEXTURE_WRAP_S, static_cast<GLint>(mode));
				break;
			case 1:
				glTexParameteri(m_texType, GL_TEXTURE_WRAP_T, static_cast<GLint>(mode));
				break;
			case 2:
				glTexParameteri(m_texType, GL_TEXTURE_WRAP_R, static_cast<GLint>(mode));
				break;
			}
		}

		void setMinFilterMode(FilterMode mode)
		{
			params.minFilterMode = static_cast<rendering::FilterMode>(mode);
			glTexParameteri(m_texType, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(mode));
		}

		void setMagFilterMode(FilterMode mode)
		{
			params.magFilterMode = static_cast<rendering::FilterMode>(mode);
			switch (mode)
			{
			case FilterMode::NEAREST:
			case FilterMode::LINEAR:
				glTexParameteri(m_texType, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mode));
				break;
			case FilterMode::NEAREST_MIPMAP_NEAREST:
			case FilterMode::NEAREST_MIPMAP_LINEAR:
			case FilterMode::LINEAR_MIPMAP_LINEAR:
			case FilterMode::LINEAR_MIPMAP_NEAREST:
				log::error("Only filter modes \"NEAREST\" and  \"LINEAR\" are supported for the \"MAG_FILTER\" parameter");
				break;
			}
		}

		void loadDataArray(unsigned char** textureData, int size)
		{
			ZoneScopedN("[OpenGL Texture] loadData()");

			GLenum internalFormat = NULL;

			switch (channels)
			{
			case 4:
				internalFormat = GL_RGBA;
				break;
			case 3:
				internalFormat = GL_RGB;
				break;
			case 2:
				internalFormat = GL_RG;
				break;
			case 1:
				internalFormat = GL_RED;
				break;
			}

			if (m_texType == GL_TEXTURE_CUBE_MAP)
			{
				for (unsigned int i = 0; i < 6; i++)
					createTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, internalFormat, textureData[i]);
			}
			else
			{
				for (unsigned int i = 0; i < size; i++)
					createTexture(m_texType, internalFormat, textureData[i]);
			}


			if (params.generateMipMaps)
			{
				glGenerateMipmap(m_texType);
			}

		}

		void loadData(unsigned char* textureData)
		{
			ZoneScopedN("[OpenGL Texture] loadData()");

			GLenum internalFormat = NULL;

			switch (channels)
			{
			case 4:
				internalFormat = GL_RGBA;
				break;
			case 3:
				internalFormat = GL_RGB;
				break;
			case 2:
				internalFormat = GL_RG;
				break;
			case 1:
				internalFormat = GL_RED;
				break;
			}

			if (m_texType == GL_TEXTURE_CUBE_MAP)
			{
				for (unsigned int i = 0; i < 6; i++)
					createTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, internalFormat, textureData);
			}
			else
				createTexture(m_texType, internalFormat, textureData);


			if (params.generateMipMaps)
			{
				glGenerateMipmap(m_texType);
			}

		}
	private:

		void createTexture(GLenum texType, GLenum internalFormat, unsigned char* textureData)
		{
			switch (m_usageType)
			{
			case static_cast<GLenum>(0):
				glTexStorage2D(texType, static_cast<GLint>(params.mipLevels), static_cast<GLint>(params.format), resolution.x, resolution.y);
				glTexSubImage2D(texType, 0, 0, 0, resolution.x, resolution.y, internalFormat, GL_UNSIGNED_BYTE, textureData);
				break;
			case GL_DEPTH_COMPONENT:
				if (textureData == nullptr)
					glTexImage2D(texType, 0, static_cast<GLint>(params.format), resolution.x, resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				else
					glTexImage2D(texType, 0, static_cast<GLint>(params.format), resolution.x, resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, textureData);
				break;
			case GL_DYNAMIC_DRAW:
				if (textureData == nullptr)
					glTexImage2D(texType, 0, static_cast<GLint>(params.format), resolution.x, resolution.y, 0, internalFormat, GL_UNSIGNED_BYTE, NULL);
				else
					glTexImage2D(texType, 0, static_cast<GLint>(params.format), resolution.x, resolution.y, 0, internalFormat, GL_UNSIGNED_BYTE, textureData);
				break;
			default:
				break;
			}

		}
	};
}