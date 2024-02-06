#pragma once
#include <unordered_map>
#include <map>
#include <memory>
#include <concepts>

#include <rsl/utilities>
#include <rsl/math>

#include "graphics/interface/definitions/definitions.hpp"
#include "graphics/pipeline/base/pipelinebase.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/components/camera.hpp"

namespace rythe::rendering
{
	class Renderer;

	template<typename Self>
	class Pipeline : public PipelineBase
	{
		friend class Renderer;
		friend struct graphics_stage_base;
	private:
		virtual void setup() =0;
	protected:
		static std::unordered_map<rsl::id_type, rsl::priority_type> m_stageIds;
		static std::map<rsl::priority_type, std::unique_ptr<graphics_stage_base>> m_stages;

	public:
		virtual ~Pipeline() = default;

		void init() override
		{
			setup();
			for (auto& [_, stage] : m_stages)
			{
				stage->RI = &RI;
				stage->pipeline = this;
			}
		}

		void render(core::transform camTransf, camera& cam) override
		{
			m_abort = false;

			for (auto& [_, stage] : m_stages)
			{
				if (!stage->isInitialized())
				{
					stage->init(camTransf, cam);
				}

				stage->render(camTransf, cam);

				if (m_abort)
					break;
			}
		}

		template<typename Type>
		static void attachStage()
		{
			auto ptr = new Type();
			m_stages.emplace(ptr->priority(), std::unique_ptr<graphics_stage_base>(ptr));
			m_stageIds.emplace(rsl::typeHash<Type>(), ptr->priority());
		}

		template<typename Stage>
		static Stage* getStage()
		{
			auto id = rsl::typeHash<Stage>();
			if (m_stageIds.count(id))
			{
				return reinterpret_cast<Stage*>(m_stages[m_stageIds[id]].get());
			}
			return nullptr;
		}
	};

	template<typename Self>
	std::map<rsl::priority_type, std::unique_ptr<graphics_stage_base>> Pipeline<Self>::m_stages;
	template<typename Self>
	std::unordered_map<rsl::id_type, rsl::priority_type> Pipeline<Self>::m_stageIds;
}