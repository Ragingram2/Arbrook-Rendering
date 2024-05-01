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

	struct mesh_renderer
	{
		//using ReflectionType = mesh_rendererImpl;
		bool enabled = true;
		bool dirty = true;
		bool instanced = false;
		bool castShadows = true;
		inputlayout layout;
		std::unordered_map<rsl::id_type, material> materials;
		material mainMaterial;
		model model;

		//mesh_renderer() = default;
		//mesh_renderer(bool instanced, bool castShadows, material mainMaterial, struct model model)
		//{
		//	this->instanced = instanced;
		//	this->castShadows = castShadows;
		//	this->mainMaterial = mainMaterial;
		//	this->model = model;
		//}
		//mesh_renderer(const mesh_renderer& _impl) : enabled(_impl.enabled), dirty(_impl.dirty), instanced(_impl.instanced), castShadows(_impl.castShadows), layout(_impl.layout), materials(_impl.materials), mainMaterial(_impl.mainMaterial), model(_impl.model) {}
		//mesh_renderer(const mesh_rendererImpl& _impl) : instanced(_impl.instanced), castShadows(_impl.castShadows), mainMaterial(_impl.mainMaterial), model(_impl.model) {}
		//~mesh_renderer() = default;


		//ReflectionType reflection() const
		//{
		//	return mesh_rendererImpl{ .instanced = instanced, .castShadows = castShadows, .mainMaterial = mainMaterial, .model = model };
		//}
	};

	struct mesh_rendererImpl
	{
		bool instanced = false;
		bool castShadows = true;
		material mainMaterial;
		model model;

		static mesh_rendererImpl from_class(const mesh_renderer& _m) noexcept
		{
			return mesh_rendererImpl{ .instanced = _m.instanced,.castShadows = _m.castShadows, .mainMaterial = _m.mainMaterial, .model = _m.model };
		}
	};

}

namespace rfl {
	namespace parsing {

		template <class ReaderType, class WriterType>
		struct Parser<ReaderType, WriterType, rythe::rendering::mesh_renderer>
			: public CustomParser<ReaderType, WriterType, rythe::rendering::mesh_renderer, rythe::rendering::mesh_rendererImpl> {};

	}  // namespace parsing
}  // namespace rfl