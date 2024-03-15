#pragma once
#include <imgui/imgui.h> 
#include <imgui/imgui_internal.h>
#include <imgui/addons/ImGuizmo/ImGuizmo.h>

#include <rsl/primitives>
#include <rsl/utilities>
#include <rsl/logging>

#include "core/logging/logging.hpp"
#include "core/components/transform.hpp"
#include "graphics/cache/cache.hpp"
#include "graphics/interface/definitions/definitions.hpp"
#include "graphics/pipeline/base/graphicsstage.hpp"
#include "graphics/components/mesh_renderer.hpp"
#include "graphics/components/camera.hpp"

namespace rythe::rendering
{
	namespace log = rsl::log;
	using guiRenderFunc = void(core::transform,camera);
	struct gui_stage : public graphics_stage<gui_stage>
	{
	private:
		static rsl::multicast_delegate<guiRenderFunc> m_onGuiRender;
	public:
		virtual void setup(core::transform camTransf, camera& cam) override;
		virtual void render(core::transform camTransf, camera& cam) override;
		virtual rsl::priority_type priority() const override;

		template <class T, void(T::* Func)(core::transform, camera)>
		static void addGuiRender(T* ptr)
		{
			m_onGuiRender.push_back<T, Func>(*ptr);
		}
	};
}