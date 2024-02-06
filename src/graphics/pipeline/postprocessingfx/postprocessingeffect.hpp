#pragma once
#include <rsl/primitives>
#include <rsl/utilities>
#include <rsl/math>
#include <rsl/containers>

#include "core/components/components.hpp"
#include "graphics/components/camera.hpp"
#include "graphics/pipeline/postprocessingfx/postprocessingeffectbase.hpp"

namespace rythe::rendering
{
	template<typename Self>
	struct post_processing_effect : public post_processing_effect_base
	{
		template<typename Stage, void (Stage::* Func)(core::transform, camera&)>
		static void addRenderPass(Stage* ptr)
		{
			renderPasses.emplace(ptr->priority(), rsl::delegate<renderPassFunc>::template create<Stage, Func>(*ptr));
		}
	};
}