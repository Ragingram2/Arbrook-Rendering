#pragma once
#include "core/modules/module.hpp"
#include "core/logging/logging.hpp"

#include "graphics/systems/rendertestsystem.hpp"
#include "graphics/systems/renderer.hpp"

namespace rythe::rendering
{
	namespace log = core::log;

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