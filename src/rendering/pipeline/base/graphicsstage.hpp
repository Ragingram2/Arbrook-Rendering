#pragma once

#include <tracy/Tracy.hpp>

#include <rsl/primitives>
#include <rsl/utilities>

#include "core/logging/logging.hpp"

#include "rendering/interface/definitions/definitions.hpp"
#include "rendering/cache/shadercache.hpp"
#include "rendering/cache/texturecache.hpp"
#include "rendering/cache/buffercache.hpp"
#include "rendering/pipeline/base/pipelinebase.hpp"
#include "rendering/pipeline/base/graphicsstagebase.hpp"
#include "rendering/components/components.hpp"

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