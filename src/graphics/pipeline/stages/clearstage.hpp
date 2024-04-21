#pragma once
#include <rsl/math>

#include "graphics/cache/texturecache.hpp"
#include "graphics/cache/materialcache.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/components/components.hpp"
#include "core/components/transform.hpp"


namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	struct clear_stage : public graphics_stage<clear_stage, core::transform,mesh_renderer>
	{
		framebuffer* mainFBO;
		framebuffer* depthFBO;
		framebuffer* depthCubeFBO;
		framebuffer* pickingFBO;

		texture_handle colorHandle;
		texture_handle mainDepthHandle;
		texture_handle depthHandle;
		texture_handle depthCubeMap;

		virtual void setup(core::transform camTransf, camera& cam) override;
		virtual void render(core::transform camTransf, camera& cam) override;
		virtual rsl::priority_type priority() const override;
		void initializeModel(mesh_renderer& renderer);
	};
}