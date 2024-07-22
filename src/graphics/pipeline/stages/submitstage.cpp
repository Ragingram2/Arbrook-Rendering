#include "graphics/pipeline/stages/submitstage.hpp"

namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	void submit_stage::setup(core::transform camTransf, camera& cam)
	{
		mainFBO = getFramebuffer("MainBuffer");
	}

	void submit_stage::render(core::transform camTransf, camera& cam)
	{
		mainFBO->unbind();
		RI->depthTest(false);
		RI->cullFace(CullMode::NONE);
		RI->updateDepthStencil();

		RI->setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		RI->clear(true);
	}

	rsl::priority_type submit_stage::priority() const { return SUBMIT_PRIORITY; }
}