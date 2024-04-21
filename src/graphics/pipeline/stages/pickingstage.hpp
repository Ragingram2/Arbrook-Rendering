#pragma once
#include <rsl/math>

#include "core/components/transform.hpp"
#include "core/assets/assethandle.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/components/components.hpp"
#include "graphics/cache/cache.hpp"
#include "graphics/data/material.hpp"

namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	struct picking_stage : public graphics_stage<picking_stage, core::transform, mesh_renderer>
	{
		buffer_handle entData;
		buffer_handle cameraBuffer;
		shader_handle pickingShader;
		framebuffer* mainFBO;
		framebuffer* pickingFBO;
		virtual void setup(core::transform, camera&) override;
		virtual void render(core::transform, camera&) override;
		virtual rsl::priority_type priority() const override;

		void initializeModel(mesh_renderer&);
		math::ivec4 convertToBase256(rsl::id_type);

	};
}