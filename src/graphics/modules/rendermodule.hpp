#pragma once
#include <rsl/logging>

#include "core/modules/module.hpp"
#include "graphics/systems/renderer.hpp"

namespace rythe::rendering
{
	namespace log = rsl::log;

	class RenderModule : public core::Module
	{
	public:
		void setup()
		{
			log::info("Initializing Rendering Module");
			reportSystem<Renderer>();
		}
	};
}