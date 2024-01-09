#pragma once
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/components/components.hpp"
#include "core/components/transform.hpp"

namespace rythe::rendering
{
	struct submit_stage : public graphics_stage<submit_stage, core::transform>
	{
		virtual void setup(core::transform camTransf, camera& cam) override
		{

		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			RI->swapBuffers();
		}

		virtual rsl::priority_type priority() override { return SUBMIT_PRIORITY; }
	};
}
