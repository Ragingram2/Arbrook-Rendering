#pragma once
#include <vector>
#include <memory>

#include "rendering/pipeline/base/pipelinebase.hpp"

namespace rythe::rendering
{
	struct pipeline_provider_base
	{
		virtual ~pipeline_provider_base() = default;
		virtual PipelineBase* get(int index) = 0;
		virtual void shutdown() = 0;
	};

	template<typename PipelineType>
	struct pipeline_provider : public pipeline_provider_base
	{
		static std::vector<std::unique_ptr<PipelineType>> m_pipelines;

		PipelineBase* get(int index) override
		{
			if (index <= m_pipelines.size() - 1)//what is this?
			{
				m_pipelines.push_back(std::make_unique<PipelineType>());
				graphics_stage_base::pipeline = m_pipelines[index].get();
				graphics_stage_base::pipeline->init();
			}
			else
				graphics_stage_base::pipeline = m_pipelines[index].get();

			return graphics_stage_base::pipeline;
		}

		void shutdown() override {}
	};
}

#include "rendering/pipeline/base/pipelineprovider.inl"