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
		buffer_handle indexBuffer;
		buffer_handle vertexBuffer;
		buffer_handle normalBuffer;
		buffer_handle uvBuffer;
		buffer_handle colorBuffer;
		buffer_handle tangentBuffer;
		buffer_handle matrixBuffer;
		ast::asset_handle<mesh> meshHandle;

		model() = default;
		model(const model& mod) : name(mod.name), indexBuffer(mod.indexBuffer), vertexBuffer(mod.vertexBuffer), normalBuffer(mod.normalBuffer), uvBuffer(mod.uvBuffer), colorBuffer(mod.colorBuffer), tangentBuffer(mod.tangentBuffer), matrixBuffer(mod.matrixBuffer), meshHandle(mod.meshHandle) { }

		void bind()
		{
			if (vertexBuffer != nullptr)
				vertexBuffer->bind();

			if (indexBuffer != nullptr)
				indexBuffer->bind();

			if (normalBuffer != nullptr)
				normalBuffer->bind();

			if (uvBuffer != nullptr)
				uvBuffer->bind();

			if (tangentBuffer != nullptr)
				tangentBuffer->bind();
		}

		void unbind()
		{
			if (vertexBuffer != nullptr)
				vertexBuffer->unbind();

			if (indexBuffer != nullptr)
				indexBuffer->unbind();

			if (normalBuffer != nullptr)
				normalBuffer->unbind();

			if (uvBuffer != nullptr)
				uvBuffer->unbind();

			if (tangentBuffer != nullptr)
				tangentBuffer->unbind();
		}
	};
	struct modelImpl
	{
		model* model;
	};

}