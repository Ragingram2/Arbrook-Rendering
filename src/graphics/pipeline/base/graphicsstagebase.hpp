#pragma once
#include <rsl/utilities>
#include <rsl/math>

#include "core/components/transform.hpp"
#include "graphics/interface/definitions/definitions.hpp"
#include "graphics/components/camera.hpp"
#include "graphics/pipeline/base/pipelinebase.hpp"
#include "graphics/interface/definitions/framebuffer.hpp"

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
		virtual rsl::priority_type priority() const { return 0; };

		bool isInitialized() { return m_initialized; }
		void init(core::transform camTransf, camera& cam)
		{
			m_initialized = true;
			setup(camTransf, cam);
		}

		framebuffer* addFramebuffer(const std::string& name) { return pipeline->addFramebuffer(name); }
		framebuffer* getFramebuffer(const std::string& name) { return getFramebuffer(rsl::nameHash(name)); }
		framebuffer* getFramebuffer(rsl::id_type nameHash) { return pipeline->getFramebuffer(nameHash); }
		bool hasFramebuffer(const std::string& name) const { return hasFramebuffer(rsl::nameHash(name)); }
		bool hasFramebuffer(rsl::id_type nameHash) const { return pipeline->hasFramebuffer(nameHash); }


	};
}