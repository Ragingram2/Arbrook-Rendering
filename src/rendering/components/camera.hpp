#pragma once
#include <rsl/math>

//#include "core/components/property.hpp"
#include "core/components/transform.hpp"
#include "core/ecs/component_family.hpp"
#include "rendering/interface/definitions/definitions.hpp"

namespace rythe::rendering
{
	struct camera_data
	{
		math::vec3 viewPosition;
		math::mat4 projection;
		math::mat4 view;
		math::mat4 model;
	};

	struct camera
	{
	public:
		math::mat4 projection;
		math::mat4 view;
		float nearZ = 1.0f;
		float farZ = 100.f;
		float fov = 60;
		
		math::mat4 calculate_view(core::transform* parent)
		{
			if (parent)
			{
				view = parent->from_world();
				return view;
			}

			//if (m_owner)
			//{
			//	if (m_owner.hasComponent<core::transform>())
			//	{
			//		auto transf = m_owner.getComponent<core::transform>();
			//		view = transf.from_world();
			//		return view;
			//	}
			//}

			return math::mat4(1.f);
		}

		math::mat4 calculate_view(math::vec3 scale, math::quat orientation, math::vec3 position)
		{
			view = math::inverse(math::compose(scale, orientation, position));
			return view;
		}

		math::mat4 calculate_projection()
		{
			auto ratio = ((float)Screen_Width) / Screen_Height;
			auto fovx = math::deg2rad(fov);
			auto invTanHalfFovx = 1.f / math::tan(fovx * 0.5f);
			auto depthScale = farZ / (farZ - nearZ);
			projection = math::float4x4(
				invTanHalfFovx, 0.f, 0.f, 0.f,
				0.f, invTanHalfFovx * ratio, 0.f, 0.f,
				0.f, 0.f, depthScale, 1.f,
				0.f, 0.f, -nearZ * depthScale, 0.f);
			return projection;
		}
	};
}