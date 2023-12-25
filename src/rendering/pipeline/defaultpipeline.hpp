#pragma once
#include "rendering/pipeline/base/pipeline.hpp"
#include "rendering/pipeline/stages/clearstage.hpp"
#include "rendering/pipeline/stages/slyboxstage.hpp"
#include "rendering/pipeline/stages/renderstage.hpp"
#include "rendering/pipeline/stages/guistage.hpp"
#include "rendering/pipeline/stages/lightrenderstage.hpp"

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

			RI.initialize(math::vec2(Screen_Width, Screen_Height), "Arbrook");
		}
	};
}