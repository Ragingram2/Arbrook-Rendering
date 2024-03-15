#include "graphics/pipeline/stages/guistage.hpp"
namespace rythe::rendering
{
	namespace log = rsl::log;
	using guiRenderFunc = void(core::transform, camera);

	rsl::multicast_delegate<guiRenderFunc> gui_stage::m_onGuiRender;

	void gui_stage::setup(core::transform camTransf, camera& cam)
	{
		RI->makeCurrent();
		auto* ctx = ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ctx->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui::SetCurrentContext(ctx);
#if RenderingAPI == RenderingAPI_OGL
		if (!ImGui_ImplGlfw_InitForOpenGL(RI->getGlfwWindow(), true))
		{
			log::error("Imgui GLFW initialization failed");
		}
		if (!ImGui_ImplOpenGL3_Init("#version 420"))
		{
			log::error("Imgui OpenGL initialization failed");
		}
#elif RenderingAPI == RenderingAPI_DX11
		if (!ImGui_ImplGlfw_InitForOther(RI->getGlfwWindow(), true))
		{
			log::error("Imgui GLFW initialization failed");
		}
		if (!ImGui_ImplDX11_Init(RI->getWindowHandle()->dev, RI->getWindowHandle()->devcon))
		{
			log::error("Imgui DX11 initialization failed");
		}
#endif
	}

	void gui_stage::render(core::transform camTransf, camera& cam)
	{
		ZoneScopedN("[Renderer] GUI Stage");
#if RenderingAPI == RenderingAPI_OGL
		ImGui_ImplOpenGL3_NewFrame();
#elif RenderingAPI == RenderingAPI_DX11
		ImGui_ImplDX11_NewFrame();
#endif
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		m_onGuiRender(camTransf, cam);

		ImGui::Render();
		auto* draw_data = ImGui::GetDrawData();
#if RenderingAPI == RenderingAPI_OGL
		ImGui_ImplOpenGL3_RenderDrawData(draw_data);
#elif RenderingAPI == RenderingAPI_DX11
		ImGui_ImplDX11_RenderDrawData(draw_data);
#endif

		ImGui::EndFrame();
		RI->swapBuffers();
	}

	rsl::priority_type gui_stage::priority() const { return UI_PRIORITY; }
}