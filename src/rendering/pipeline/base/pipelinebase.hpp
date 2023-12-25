#pragma once
#include <rsl/utilities>
#include <rsl/math>

#include "core/components/transform.hpp"
#include "rendering/interface/definitions/definitions.hpp"
#include "rendering/components/camera.hpp"

namespace rythe::rendering
{
	class PipelineBase
	{
	protected:
		bool m_abort = false;
	public:
		RenderInterface RI;//This should be the only real version
		virtual void init() =0;
		virtual void render(core::transform camTransf, camera& cam) =0;
		virtual void shutdown() {}

		void abort()
		{
			m_abort = true;
		}
	};
}
