#pragma once
#include <unordered_map>

#include <rsl/primitives>
#include <rfl.hpp>
#include <rfl/json.hpp>

#include "core/assets/assethandle.hpp"
#include "core/components/property.hpp"
#include "graphics/data/material.hpp"
#include "graphics/data/model.hpp"



namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	struct mesh_rendererImpl
	{
		bool instanced = false;
		bool castShadows = true;
		materialImpl mainMaterial;
		modelImpl model;

		//static mesh_rendererImpl from_class(const mesh_renderer& _m) noexcept
		//{
		//	return mesh_rendererImpl{ .instanced = _m.instanced,.castShadows = _m.castShadows, .mainMaterial = _m.mainMaterial, .model = _m.model };
		//}
	};

	struct mesh_renderer
	{
		bool enabled = true;
		bool dirty = true;
		bool instanced = false;
		bool castShadows = true;
		inputlayout layout;
		std::unordered_map<rsl::id_type, material> materials;
		material mainMaterial;
		model model;

		mesh_rendererImpl reflectedType;

		mesh_rendererImpl& reflection()
		{
			reflectedType.castShadows = castShadows;
			reflectedType.instanced = instanced;
			reflectedType.mainMaterial.mat = &mainMaterial;
			reflectedType.model.model = &model;
			return reflectedType;
		}
	};
}
//
//namespace rfl {
//	namespace parsing {
//
//		template <class ReaderType, class WriterType>
//		struct Parser<ReaderType, WriterType, rythe::rendering::mesh_renderer>
//			: public CustomParser<ReaderType, WriterType, rythe::rendering::mesh_renderer, rythe::rendering::mesh_rendererImpl> {};
//
//	}  // namespace parsing
//}  // namespace rfl