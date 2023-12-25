#pragma once
#include <memory>

#include <rsl/logging>
#include <rsl/math>

#include "core/ecs/ecs.hpp"
#include "core/systems/system.hpp"
#include "rendering/pipeline/base/graphicsstage.hpp"
#include "rendering/pipeline/base/pipelineprovider.hpp"
#include "rendering/pipeline/base/pipeline.hpp"
#include "rendering/pipeline/defaultpipeline.hpp"
#include "rendering/components/components.hpp"


namespace rythe::rendering
{
	class Renderer : public core::System<Renderer,core::transform, camera>
	{
	public:
		static std::unique_ptr<pipeline_provider_base> provider;
		static PipelineBase* pipeline;
		static RenderInterface* RI;

		Renderer() : core::System<Renderer, core::transform, camera>()
		{
			setPipeline<DefaultPipeline>();
		}
		virtual ~Renderer() = default;

		void setup()
		{
			if (!glfwInit())
				return;

			log::info("Initializing Render Pipeline");

			pipeline = provider->get(0);
			RI = &pipeline->RI;

			if (!RI->getGlfwWindow())
			{
				glfwTerminate();
				log::error("Window initialization failed");
				return;
			}
		}

		void update()
		{
			ZoneScopedN("Render Frame");
			if (RI->shouldWindowClose())
			{
				rythe::core::events::exit evnt(0);
				raiseEvent(evnt);
				return;
			}

			if (m_filter.size() < 1) return;

			auto& camEnt = getCameraEntity();
			auto& transf = camEnt.getComponent<core::transform>();
			auto& cam = camEnt.getComponent<camera>();
			cam.calculate_projection();
			pipeline->render(transf, getCamera());

			//RI->pollEvents();
		}

		void shutdown()
		{
			pipeline->shutdown();
		}

		core::ecs::entity& getCameraEntity()
		{
			return m_filter[0];
		}

		camera& getCamera()
		{
			return m_filter[0].getComponent<camera>();
		}

		template<typename Type>
		static void setPipeline()
		{
			provider = std::make_unique<pipeline_provider<Type>>();
			//pipeline = provider->get(0);
			//RI = &pipeline->RI;
		}

		static PipelineBase* getCurrentPipeline()
		{
			return pipeline;
		}
	};

}