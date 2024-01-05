#pragma once
#include "core/assets/assethandle.hpp"
#include "graphics/interface/definitions/shader.hpp"
#include "graphics/interface/definitions/texture.hpp"

namespace ast = rythe::core::assets;
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
		ast::asset_handle<texture> diffuse;
		ast::asset_handle<texture> specular;
		ast::asset_handle<shader> shader;

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