#pragma once

#include "core/assets/assethandle.hpp"

#include "graphics/data/bufferhandle.hpp"
#include "graphics/data/shaderhandle.hpp"
#include "graphics/data/mesh.hpp"
#include "graphics/data/material.hpp"
#include "graphics/interface/definitions/buffer.hpp"
#include "graphics/interface/definitions/inputlayout.hpp"
#include "graphics/cache/buffercache.hpp"
#include "graphics/components/light.hpp"
#include "graphics/components/camera.hpp"

namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	struct model
	{
		std::string name;
		inputlayout layout;
		buffer_handle indexBuffer;
		buffer_handle vertexBuffer;
		buffer_handle normalBuffer;
		buffer_handle uvBuffer;
		buffer_handle colorBuffer;
		buffer_handle tangentBuffer;
		buffer_handle matrixBuffer;
		ast::asset_handle<mesh> meshHandle;
		ast::asset_handle<material> matHandle;

		model() = default;
		model(const model& mod) : name(mod.name), indexBuffer(mod.indexBuffer), vertexBuffer(mod.vertexBuffer), normalBuffer(mod.normalBuffer), uvBuffer(mod.uvBuffer), colorBuffer(mod.colorBuffer), tangentBuffer(mod.tangentBuffer), matrixBuffer(mod.matrixBuffer), meshHandle(mod.meshHandle), matHandle(mod.matHandle) { }

		//void initialize(bool instanced)
		//{
		//	layout.release();
		//	layout.initialize(1, matHandle->shader);
		//	layout.bind();

		//	vertexBuffer = BufferCache::createVertexBuffer<math::vec4>(std::format("{}-Vertex Buffer", meshHandle->name), 0, UsageType::STATICDRAW, meshHandle->vertices);
		//	layout.setAttributePtr(vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);

		//	indexBuffer = BufferCache::createIndexBuffer(std::format("{}-Index Buffer", meshHandle->name), UsageType::STATICDRAW, meshHandle->indices);

		//	if (meshHandle->normals.size() > 0)
		//	{
		//		normalBuffer = BufferCache::createVertexBuffer<math::vec3>(std::format("{}-Normal Buffer", meshHandle->name), 1, UsageType::STATICDRAW, meshHandle->normals);
		//		layout.setAttributePtr(normalBuffer, "NORMAL", 0, FormatType::RGB32F, 1, sizeof(math::vec3), 0);
		//	}

		//	if (meshHandle->texCoords.size() > 0)
		//	{
		//		uvBuffer = BufferCache::createVertexBuffer<math::vec2>(std::format("{}-UV Buffer", meshHandle->name), 2, UsageType::STATICDRAW, meshHandle->texCoords);
		//		layout.setAttributePtr(uvBuffer, "TEXCOORD", 0, FormatType::RG32F, 2, sizeof(math::vec2), 0);
		//	}

		//	if (instanced)
		//	{
		//		matrixBuffer = BufferCache::createBuffer<math::mat4>(std::format("{}-Matrix Buffer", meshHandle->name), TargetType::VERTEX_BUFFER);
		//		layout.setAttributePtr(matrixBuffer, "MODEL", 1, FormatType::RGBA32F, 3, sizeof(math::mat4), 0.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
		//		layout.setAttributePtr(matrixBuffer, "MODEL", 2, FormatType::RGBA32F, 3, sizeof(math::mat4), 1.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
		//		layout.setAttributePtr(matrixBuffer, "MODEL", 3, FormatType::RGBA32F, 3, sizeof(math::mat4), 2.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
		//		layout.setAttributePtr(matrixBuffer, "MODEL", 4, FormatType::RGBA32F, 3, sizeof(math::mat4), 3.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
		//	}
		//	layout.submitAttributes();
		//}

		void bind()
		{
			layout.bind();
			if (vertexBuffer != nullptr)
				vertexBuffer->bind();

			if (indexBuffer != nullptr)
				indexBuffer->bind();

			if (normalBuffer != nullptr)
				normalBuffer->bind();

			if (uvBuffer != nullptr)
				uvBuffer->bind();
		}
	};

}