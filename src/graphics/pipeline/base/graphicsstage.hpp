#pragma once

#include <tracy/Tracy.hpp>

#include <rsl/primitives>
#include <rsl/utilities>

#include "core/logging/logging.hpp"

#include "graphics/interface/definitions/definitions.hpp"
#include "graphics/cache/shadercache.hpp"
#include "graphics/cache/texturecache.hpp"
#include "graphics/cache/buffercache.hpp"
#include "graphics/pipeline/base/pipelinebase.hpp"
#include "graphics/pipeline/base/graphicsstagebase.hpp"
#include "graphics/components/components.hpp"

namespace rythe::rendering
{
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