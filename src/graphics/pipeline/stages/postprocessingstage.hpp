#pragma once
#include <rsl/primitives>
#include <rsl/utilities>
#include <rsl/math>

#include "core/events/defaults/component_event.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/interface/definitions/framebuffer.hpp"


namespace rythe::rendering
{
	struct ppfx_base
	{
	private:
		bool m_initialized = false;
	public:
		rsl::multicast_delegate<void(core::transform, camera&)> renderPasses;//this will call any rendering passes we want to happen during the ppfx
	protected:
		virtual void setup() = 0;
		virtual rsl::priority_type priority() const { return 0; }
	private:
	public:
		virtual void render() = 0;
		void init()
		{
			m_initialized = true;
			setup();
		}
		bool isInitialized() { return m_initialized; }
	};

	struct ppfx : public ppfx_base
	{
		virtual void setup() override
		{
			//do the setup
		}

		virtual void render() override
		{
			//do the rendering
		}

		virtual rsl::priority_type priority() const override { return 0; }
	};

	struct post_processing_stage : public graphics_stage<post_processing_stage>
	{
		std::map<rsl::priority_type, std::unique_ptr<ppfx_base>> ppfxs;
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			for (auto& [priority, ppfx] : ppfxs)
			{
				if (!ppfx->isInitialized())
				{
					ppfx->init();
				}
			}
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			ZoneScopedN("[Renderer] Post Processing Stage");

			for (auto& [priority, ppfx] : ppfxs)
			{
				if (!ppfx->isInitialized())
				{
					ppfx->init();
					continue;
				}

				ppfx->render();
			}
		}

		template<typename Type>
		static void addEffect()
		{
			auto ptr = new Type();
			ppfxs.emplace(ptr->priority(), std::make_unique<ppfx_base>(std::move(ptr)));
		}

		virtual rsl::priority_type priority() const override { return POST_FX_PRIORITY; }
	};
}