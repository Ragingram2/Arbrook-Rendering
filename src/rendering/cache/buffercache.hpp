#pragma once
#include <string>
#include <memory>
#include <unordered_map>

#include "core/logging/logging.hpp"

#include "rendering/data/bufferhandle.hpp"
#include "rendering/interface/definitions/buffer.hpp"
#include "rendering/interface/definitions/enumtypes.hpp"


namespace rythe::rendering
{
	class BufferCache
	{
	private:
		static std::unordered_map<std::string, std::unique_ptr<buffer>> m_buffers;
	public:
		template<typename elementType>
		static buffer_handle createVertexBuffer(const std::string& name, int bindId, UsageType usage, std::vector<elementType> data = std::vector<elementType>());
		template<typename elementType>
		static buffer_handle createConstantBuffer(const std::string& name, int bindId, UsageType usage, std::vector<elementType> data = std::vector<elementType>());
		static buffer_handle createIndexBuffer(const std::string& name, UsageType usage, std::vector<unsigned int> data = std::vector<unsigned int>());
		template<typename elementType>
		static buffer_handle createBuffer(const std::string& name, TargetType target, UsageType usage, std::vector<elementType> data = std::vector<elementType>());
		template<typename elementType>
		static buffer_handle createBuffer(const std::string& name, TargetType target, UsageType usage = UsageType::STATICDRAW, elementType* data = nullptr, int size = 1);
		static buffer_handle getBuffer(const std::string& name);
		static void deleteBuffer(const std::string& name);
	};

	template<typename elementType>
	inline buffer_handle BufferCache::createVertexBuffer(const std::string& name, int bindId, UsageType usage, std::vector<elementType> data)
	{
		auto handle = createBuffer(name, TargetType::VERTEX_BUFFER, usage, data);
		handle->getImpl().bindId = bindId;
		return handle;
	}

	template<typename elementType>
	inline buffer_handle BufferCache::createConstantBuffer(const std::string& name, int bindId, UsageType usage, std::vector<elementType> data)
	{
		auto handle = createBuffer(name,TargetType::CONSTANT_BUFFER,usage,data);
		handle->getImpl().bindId = bindId;
		return handle;
	}

	template<typename elementType>
	inline buffer_handle BufferCache::createBuffer(const std::string& name, TargetType target, UsageType usage, std::vector<elementType> data)
	{
		int size = 1;
		if (data.size() > 1)
			size = data.size();
		return { createBuffer(name, target, usage,data.data(), size) };
	}

	template<typename elementType>
	inline buffer_handle BufferCache::createBuffer(const std::string& name, TargetType target, UsageType usage, elementType* data, int size)
	{
		if (m_buffers.contains(name))
		{
			if (data)
			{
				m_buffers[name]->bufferData<elementType>(data, size);
			}
			return m_buffers[name].get();
		}

		auto buff = m_buffers.emplace(name, std::make_unique<buffer>()).first->second.get();
		buff->m_impl.name = name;
		buff->initialize<elementType>(target, usage, size);
		if (data)
		{
			buff->bufferData<elementType>(data, size);
		}
		return { buff };
	}
}