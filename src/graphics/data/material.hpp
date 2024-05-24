#pragma once
#include <any>

#include "graphics/data/shaderhandle.hpp"
#include "graphics/data/texturehandle.hpp"
#include "graphics/interface/definitions/shader.hpp"
#include "graphics/interface/definitions/texture.hpp"

namespace rythe::rendering
{
	struct material_data
	{
		math::vec4 diffuseColor;
		unsigned int hasAlbedo = 0;
		unsigned int hasRoughness = 0;
		unsigned int hasNormal = 0;
		unsigned int hasHeight = 0;
		unsigned int hasMetallic = 0;
		unsigned int hasAmbientOcclusion = 0;
		unsigned int hasEmissive = 0;
	};

	enum class ParamType
	{
		None,
		Texture,
		Uniform
	};

	struct uniform
	{
		std::any data;
	};

	struct material_parameter_base
	{
		rsl::id_type bufferRegister = 0;
		ParamType type = ParamType::None;
		std::string name;
	};

	template<typename valueType>
	struct material_parameter : material_parameter_base
	{
		valueType value;
	};

	template<>
	struct material_parameter<std::string> : material_parameter_base
	{
		std::string value;
	};



	struct material
	{
	private:
		shader_handle m_shader;
		std::unordered_map<TextureSlot, texture_handle> m_textures = std::unordered_map<TextureSlot,texture_handle>();
		std::unordered_map<std::string, material_parameter<uniform>> m_uniforms = std::unordered_map<std::string, material_parameter<uniform>>();

	public:
		std::string name;
		material_data data;

		operator bool() const { return (bool)m_shader; }

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
			for (auto [name, param] : m_uniforms)
			{
				m_shader->setUniform(name, param.bufferRegister, &param.value.data);
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

		template<typename elementType>
		void setUniform(const std::string& name, elementType data[], int location = 0)
		{
			if (m_shader == nullptr)
			{
				log::error("No shader is connected to this material, cannot set uniform \"{}\"", name);
				return;
			}

			if (!m_uniforms.contains(name))
			{
				//if the uniform name is not in the list, this is usually because the uniform we are setting is actually a vertex buffer (camera buffer, light buffer etc)
				m_shader->setUniform(name, location, data);
			}
			else
			{
				m_uniforms[name].value.data = data;
				//m_shader->setUniform(name, m_uniforms[name].bufferRegister, data);
			}
		}

		void addTexture(TextureSlot slot, texture_handle handle)
		{
			if (!m_textures.contains(slot))
				m_textures.emplace(slot, handle);
			else
				m_textures[slot] = handle;
		}

		void addUniform(const std::string& name, material_parameter<uniform> value)
		{
			if (!m_uniforms.contains(name))
				m_uniforms.emplace(name, value);
			else
				m_uniforms[name] = value;
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

	struct materialImpl
	{
		material* mat;
	};

}