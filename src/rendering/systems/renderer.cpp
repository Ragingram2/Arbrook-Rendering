#include "rendering/systems/renderer.hpp"

namespace rythe::rendering
{
	std::unique_ptr<pipeline_provider_base> Renderer::provider;
	PipelineBase* Renderer::pipeline;
	RenderInterface* Renderer::RI;
}