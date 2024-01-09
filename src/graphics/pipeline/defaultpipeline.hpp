#pragma once
#include "graphics/pipeline/base/pipeline.hpp"
#include "graphics/pipeline/stages/clearstage.hpp"
#include "graphics/pipeline/stages/slyboxstage.hpp"
#include "graphics/pipeline/stages/renderstage.hpp"
#include "graphics/pipeline/stages/guistage.hpp"
#include "graphics/pipeline/stages/lightrenderstage.hpp"
#include "graphics/pipeline/stages/submitstage.hpp"

namespace rythe::rendering
{

	class DefaultPipeline : public Pipeline<DefaultPipeline>
	{
		virtual void setup() override
		{
			attachStage<clear_stage>();
			attachStage<skybox_stage>();
			attachStage<light_render_stage>();
			attachStage<render_stage>();
			attachStage<gui_stage>();
			attachStage<submit_stage>();

			RI.initialize(math::vec2(Screen_Width, Screen_Height), "Arbrook");
		}
	};
}