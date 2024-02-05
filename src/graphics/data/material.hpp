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
		std::string name;
		material_data data;
		texture_handle texture0;
		texture_handle texture1;
		shader_handle shader;

		material() = default;
		material(const material& mat) : texture0(mat.texture0), texture1(mat.texture1), shader(mat.shader) {}

		void bind()
		{
			if (shader != nullptr)
				shader->bind();
			else
				log::warn("[Material] Shader handle is null, cannot bind");
			if (texture0 != nullptr)
			{
				texture0->bind(TEXTURE0_SLOT);
			}
			if (texture1 != nullptr)
			{
				texture1->bind(TEXTURE1_SLOT);
			}
		}

		void unbind()
		{
			//if (shader != nullptr)
			//	shader->unbind();
			//else
			//	log::warn("[Material] Shader handle is null, cannot bind");
			if (texture0 != nullptr)
			{
				texture0->unbind(TEXTURE0_SLOT);
			}
			if (texture1 != nullptr)
			{
				texture1->unbind(TEXTURE1_SLOT);
			}
		}
	};

}