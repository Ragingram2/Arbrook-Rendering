#pragma once
#include <rsl/utilities>
#include <rsl/math>

#include "core/components/transform.hpp"
#include "rendering/interface/definitions/definitions.hpp"
#include "rendering/components/camera.hpp"
#include "rendering/pipeline/base/pipelinebase.hpp"

namespace rythe::rendering
{
	struct graphics_stage_base
	{
	private:
		bool m_initialized = false;
	public:
		static PipelineBase* pipeline;
		RenderInterface* RI;

	protected:
		virtual void setup(core::transform camTransf, camera& cam) = 0;
		virtual void shutdown_impl() = 0;
	public:
		virtual ~graphics_stage_base() = default;
		virtual void render(core::transform camTransf, camera& cam) = 0;
		virtual rsl::priority_type priority() { return 0; };

		bool isInitialized() { return m_initialized; }
		void init(core::transform camTransf, camera& cam)
		{
			m_initialized = true;
			setup(camTransf, cam);
		}
	};
}