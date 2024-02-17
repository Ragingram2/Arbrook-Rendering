#pragma once
#include "graphics/data/shaderhandle.hpp"
#include "graphics/data/texturehandle.hpp"
#include "graphics/interface/definitions/shader.hpp"
#include "graphics/interface/definitions/texture.hpp"

namespace rythe::rendering
{
	struct material_data
	{
		float shininess = 32.0f;
	};

	struct material
	{
	private:
		shader_handle m_shader;
		std::unordered_map<TextureSlot, texture_handle> m_textures;

	public:
		std::string name;
		material_data data;

		material() = default;
		material(const material& mat) : name(mat.name), data(mat.data), m_textures(mat.m_textures), m_shader(mat.m_shader) {}

		void bind()
		{
			if (m_shader != nullptr)
				m_shader->bind();
			else
				log::warn("[Material] Shader handle is null, cannot bind");

			for (auto [slot, handle] : m_textures)
			{
				handle->bind(slot);
			}
		}

		shader_handle getShader()
		{
			return m_shader;
		}

		void setShader(shader_handle handle)
		{
			m_shader = handle;
		}

		void addTexture(TextureSlot slot, texture_handle handle)
		{
			m_textures.emplace(slot, handle);
		}

		void unbind()
		{
			if (m_shader != nullptr)
				m_shader->unbind();
			else
				log::warn("[Material] Shader handle is null, cannot unbind");

			for (auto [slot, handle] : m_textures)
			{
				handle->unbind(slot);
			}
		}
	};

}