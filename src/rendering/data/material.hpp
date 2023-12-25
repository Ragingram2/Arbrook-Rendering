#pragma once
#include "rendering/data/shaderhandle.hpp"
#include "rendering/data/texturehandle.hpp"

namespace rythe::rendering
{
	struct material_data
	{
		float shininess = 32.0f;
	};

	struct material
	{
		std::string name;
		material_data data;
		texture_handle diffuse;
		texture_handle specular;
		shader_handle shader;

		material() = default;
		material(const material& mat) : diffuse(mat.diffuse), specular(mat.specular), shader(mat.shader) {}

		void bind()
		{
			if (shader != nullptr)
				shader->bind();
			else
				log::warn("[Material] Shader handle is null, cannot bind");
			if (diffuse != nullptr)
			{
				diffuse->bind(TextureSlot::TEXTURE0);
			}
			if (specular != nullptr)
			{
				specular->bind(TextureSlot::TEXTURE1);
			}
		}
	};

}