#pragma once
#include "core/components/transform.hpp"
#include "core/assets/assethandle.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/components/components.hpp"
#include "graphics/cache/materialcache.hpp"
#include "graphics/cache/modelcache.hpp"
#include "graphics/data/model.hpp"

namespace rythe::rendering
{
	struct skybox_stage : public graphics_stage<skybox_stage, skybox_renderer>
	{
		framebuffer* mainFBO;
		buffer_handle cameraBuffer;
		ast::asset_handle<material> skyboxMat;
		model cubeHandle;
		inputlayout layout;
		virtual void setup(core::transform camTransf, camera& cam) override;
		virtual void render(core::transform camTransf, camera& cam) override;
		virtual rsl::priority_type priority() const override;
		void initializeSkyboxModel();
	};
}