#pragma once

#include <rsl/primitives>
#include <rsl/utilities>

#include "core/systems/system.hpp"
#include "core/utils/profiler.hpp"
#include "graphics/interface/definitions/definitions.hpp"
#include "graphics/cache/buffercache.hpp"
#include "graphics/cache/importers/shaderimporter.hpp"
#include "graphics/cache/importers/textureimporter.hpp"
#include "graphics/pipeline/base/pipelinebase.hpp"
#include "graphics/pipeline/base/graphicsstagebase.hpp"
#include "graphics/components/components.hpp"

namespace rythe::rendering
{
	struct light_info
	{
		int index = 0;
		int count = 0;
	};

	inline light_info lightInfo;

	template<typename Self, typename... componentTypes>
	struct graphics_stage : public graphics_stage_base, protected core::System<Self, componentTypes...>
	{
		void shutdown_impl() override
		{
			if constexpr (rsl::has_shutdown_v<Self, void()>)
			{
				static_cast<Self*>(this)->shutdown();
			}
		}
	};
}