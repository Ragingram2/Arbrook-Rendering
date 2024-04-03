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

	struct material;

	struct material_parameter_base
	{
	protected:
		std::string m_name;
		rsl::id_type m_id;
		rsl::id_type m_typeId;
		rsl::id_type m_location;
		material_parameter_base(const std::string& name, rsl::id_type location, rsl::id_type typeId)
			: m_name(name),
			m_id(rsl::nameHash(name)),
			m_typeId(typeId),
			m_location(location) { }

	public:
		rsl::id_type type() { return m_typeId; }
		std::string getName() const { return m_name; }

		virtual void apply(shader_handle& shader) = 0;
	};

	template<typename T>
	struct material_parameter : public material_parameter_base
	{
		friend struct materal;
	private:
		T m_value;

		virtual void apply(shader_handle& shader) override
		{
			//shader.get_uniform<T>(m_id).set_value(m_value);
		}

	public:
		material_parameter(const std::string& name, rsl::id_type location)
			: material_parameter_base(name, location, rsl::typeHash<T>()),
			m_value()
		{
		}

		void set_value(const T& value) { m_value = value; }
		T get_value() const { return m_value; }

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