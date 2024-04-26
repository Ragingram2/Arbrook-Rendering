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
		float shininess = 32.0f;
	};

	enum class ParamType
	{
		None,
		Texture,
		Uniform
	};

	struct uniform
	{
		//std::unordered_map<std::string, std::variant<int, float, double, bool, unsigned int, math::ivec2, math::vec2, math::dvec2, math::bvec2, math::uvec2, math::ivec3, math::vec3, math::dvec3, math::bvec3, math::uvec3, math::ivec4, math::vec4, math::dvec4, math::bvec4, math::uvec4, math::mat2, math::mat3, math::mat4, math::dmat2, math::dmat3, math::dmat4 >> data;
		std::any data;
	};

	struct material_parameter_base
	{
		rsl::id_type bufferRegister = 0;
		ParamType type = ParamType::None;
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
		std::unordered_map<TextureSlot, texture_handle> m_textures;
		std::unordered_map<std::string, material_parameter<uniform>> m_uniforms;

	public:
		std::string name;
		material_data data;

		material() = default;
		material(const material& mat) : m_shader(mat.m_shader), m_textures(mat.m_textures), m_uniforms(mat.m_uniforms), name(mat.name), data(mat.data) {}

		material& operator= (const material& mat)
		{
			m_shader = mat.m_shader;
			m_textures = mat.m_textures;
			name = mat.name;
			data = mat.data;
			return *this;
		}
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

}