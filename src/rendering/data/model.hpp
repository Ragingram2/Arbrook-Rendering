#pragma once

#include "rendering/data/meshhandle.hpp"
#include "rendering/data/shaderhandle.hpp"
#include "rendering/data/bufferhandle.hpp"
#include "rendering/interface/definitions/buffer.hpp"
#include "rendering/interface/definitions/inputlayout.hpp"
#include "rendering/cache/buffercache.hpp"
#include "rendering/components/light.hpp"
#include "rendering/components/camera.hpp"

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
		mesh_handle meshHandle;


		model() = default;
		model(const model& mod) : name(mod.name), indexBuffer(mod.indexBuffer), vertexBuffer(mod.vertexBuffer), normalBuffer(mod.normalBuffer), uvBuffer(mod.uvBuffer), colorBuffer(mod.colorBuffer), tangentBuffer(mod.tangentBuffer), matrixBuffer(mod.matrixBuffer) { }

		void initialize(shader_handle shader, mesh_handle handle, bool instanced)
		{
			meshHandle = handle;
			layout.release();
			layout.initialize(1, shader);
			layout.bind();

			vertexBuffer = BufferCache::createVertexBuffer<math::vec4>(std::format("{}-Vertex Buffer", handle->name), 0, UsageType::STATICDRAW, meshHandle->vertices);
			layout.setAttributePtr(vertexBuffer, "POSITION", 0, FormatType::RGBA32F, 0, sizeof(math::vec4), 0);

			indexBuffer = BufferCache::createIndexBuffer(std::format("{}-Index Buffer", handle->name), UsageType::STATICDRAW, meshHandle->indices);

			if (meshHandle->normals.size() > 0)
			{
				normalBuffer = BufferCache::createVertexBuffer<math::vec3>(std::format("{}-Normal Buffer", handle->name), 1, UsageType::STATICDRAW, meshHandle->normals);
				layout.setAttributePtr(normalBuffer, "NORMAL", 0, FormatType::RGB32F, 1, sizeof(math::vec3), 0);
			}

			if (meshHandle->texCoords.size() > 0)
			{
				uvBuffer = BufferCache::createVertexBuffer<math::vec2>(std::format("{}-UV Buffer", handle->name), 2, UsageType::STATICDRAW, meshHandle->texCoords);
				layout.setAttributePtr(uvBuffer, "TEXCOORD", 0, FormatType::RG32F, 2, sizeof(math::vec2), 0);
			}

			if (instanced)
			{
				matrixBuffer = BufferCache::createBuffer<math::mat4>(std::format("{}-Matrix Buffer", handle->name), TargetType::VERTEX_BUFFER);
				layout.setAttributePtr(matrixBuffer, "MODEL", 1, FormatType::RGBA32F, 3, sizeof(math::mat4), 0.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
				layout.setAttributePtr(matrixBuffer, "MODEL", 2, FormatType::RGBA32F, 3, sizeof(math::mat4), 1.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
				layout.setAttributePtr(matrixBuffer, "MODEL", 3, FormatType::RGBA32F, 3, sizeof(math::mat4), 2.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
				layout.setAttributePtr(matrixBuffer, "MODEL", 4, FormatType::RGBA32F, 3, sizeof(math::mat4), 3.f * sizeof(math::vec4), InputClass::PER_INSTANCE, 1);
			}
			layout.submitAttributes();
		}

		void bind()
		{
			layout.bind();
			if (vertexBuffer != nullptr)
				vertexBuffer->bind();
			//vertexBuffer->bufferData(meshHandle->vertices.data(), meshHandle->vertices.size());

			if (indexBuffer != nullptr)
				indexBuffer->bind();
			//indexBuffer->bufferData(meshHandle->indices.data(), meshHandle->indices.size());

			if (normalBuffer != nullptr)
				normalBuffer->bind();
			//normalBuffer->bufferData(meshHandle->normals.data(), meshHandle->normals.size());

			if (uvBuffer != nullptr)
				uvBuffer->bind();
			//uvBuffer->bufferData(meshHandle->texCoords.data(), meshHandle->texCoords.size());

		}

		//void unbind()
		//{
		//	layout.unbind();
		//	vertexBuffer.unbind();
		//}
	};

}