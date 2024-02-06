#pragma once
#include "graphics/pipeline/base/pipeline.hpp"
#include "graphics/pipeline/stages/clearstage.hpp"
#include "graphics/pipeline/stages/slyboxstage.hpp"
#include "graphics/pipeline/stages/renderstage.hpp"
#include "graphics/pipeline/stages/guistage.hpp"
#include "graphics/pipeline/stages/postprocessingstage.hpp"
#include "graphics/pipeline/stages/lightrenderstage.hpp"
#include "graphics/pipeline/stages/submitstage.hpp"
#include "graphics/pipeline/postprocessingfx/fx/testfx.hpp"

namespace rythe::rendering
{
	class DefaultPipeline : public Pipeline<DefaultPipeline>
	{
		void setup() override
		{
			attachStage<clear_stage>();
			attachStage<skybox_stage>();
			attachStage<light_render_stage>();
			attachStage<render_stage>();
			attachStage<post_processing_stage>();
			attachStage<gui_stage>();
			attachStage<submit_stage>();

			test_ppfx::addRenderPass<render_stage, &render_stage::render>(getStage<render_stage>());
			post_processing_stage::addEffect<test_ppfx>();

			RI.initialize(math::vec2(Screen_Width, Screen_Height), "Arbrook");
		}
	};
}