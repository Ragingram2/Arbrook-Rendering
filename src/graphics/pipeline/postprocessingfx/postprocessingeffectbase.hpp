#pragma once
#include <map>

#include <rsl/primitives>
#include <rsl/utilities>
#include <rsl/math>
#include <rsl/containers>

#include "core/components/components.hpp"
#include "graphics/components/camera.hpp"

namespace rythe::rendering
{
	using renderPassFunc = void(core::transform, camera&);
	struct post_processing_effect_base
	{
	private:
		bool m_initialized = false;
	public:
		static std::map<rsl::priority_type, rsl::delegate<renderPassFunc>> renderPasses;
	protected:
		virtual void setup() = 0;
		virtual rsl::priority_type priority() const { return 0; }
	public:
		virtual void render(framebuffer*, RenderInterface*, core::transform, camera&) = 0;
		virtual ~post_processing_effect_base() = default;
		void init()
		{
			m_initialized = true;
			setup();
		}
		bool isInitialized() { return m_initialized; }
	};

	inline std::map<rsl::priority_type, rsl::delegate<renderPassFunc>> post_processing_effect_base::renderPasses;
}