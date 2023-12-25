#include "rendering/pipeline/base/pipelineprovider.hpp"
#pragma once

namespace rythe::rendering
{
	template<typename PipelineType>
	std::vector<std::unique_ptr<PipelineType>> pipeline_provider<PipelineType>::m_pipelines;
}