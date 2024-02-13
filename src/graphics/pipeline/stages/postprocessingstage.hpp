#pragma once
#include <map>
#include <memory>

#include <rsl/primitives>
#include <rsl/utilities>
#include <rsl/math>
#include <rsl/containers>

#include "graphics/interface/definitions/definitions.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/pipeline/postprocessingfx/postprocessingeffect.hpp"
#include "graphics/pipeline/postprocessingfx/fx/testfx.hpp"

namespace rythe::rendering
{
	struct post_processing_stage : public graphics_stage<post_processing_stage>
	{
		static std::map<rsl::priority_type, std::unique_ptr<post_processing_effect_base>> ppfxs;
		virtual void setup(core::transform camTransf, camera& cam) override
		{
			for (auto& [priority, ppfx] : ppfxs)
			{
				if (!ppfx->isInitialized())
				{
					ppfx->init(pipeline, RI);
				}
			}
		}

		virtual void render(core::transform camTransf, camera& cam) override
		{
			ZoneScopedN("[Renderer] [Post Processing Stage] Render");

			for (auto& [priority, ppfx] : ppfxs)
			{
				if (!ppfx->isInitialized())
				{
					ppfx->init(pipeline, RI);
					continue;
				}

				ppfx->render(pipeline, RI, camTransf, cam);
			}
		}

		template<typename Type>
		static void addEffect()
		{
			auto ptr = new Type();
			ppfxs.emplace(ptr->priority(), std::unique_ptr<post_processing_effect_base>(reinterpret_cast<post_processing_effect_base*>(ptr)));
		}

		virtual rsl::priority_type priority() const override { return POST_FX_PRIORITY; }
	};

	inline std::map<rsl::priority_type, std::unique_ptr<post_processing_effect_base>> post_processing_stage::ppfxs;
}