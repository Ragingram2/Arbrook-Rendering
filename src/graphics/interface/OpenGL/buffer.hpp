#pragma once
#include <vector>
#include <concepts>

#include <rsl/primitives>
#include <rsl/logging>

#include "core/utils/profiler.hpp"

#include "graphics/interface/OpenGL/oglincludes.hpp"
#include "graphics/interface/config.hpp"
#include EnumTypes_HPP_PATH

namespace rythe::rendering
{
	template<typename APIType>
	struct Ibuffer;
	namespace internal
	{
		struct buffer;
	}
}

namespace rythe::rendering::internal
{
	namespace log = rsl::log;
	struct buffer
	{
		friend struct Ibuffer<internal::buffer>;
	public:
		unsigned int id;
		std::string name;
		unsigned int bindId = 0;
	private:
		BufferType m_type;
		UsageType m_usage;
		unsigned int m_slot;
		unsigned int m_offset;
		unsigned int m_size = 0;
		unsigned int m_elementSize;
	public:

		template<typename elementType>
		void initialize(BufferType target, UsageType usage, int size, elementType* data)
		{
			ZoneScopedN("[OpenGL Buffer] initialize()");
			m_type = target;
			m_usage = usage;
			m_size = size;
			m_elementSize = sizeof(elementType);

			createBuffer(data);
		}

		void bind()
		{
			ZoneScopedN("[OpenGL Buffer] bind()");
			if (m_type == BufferType::CONSTANT_BUFFER)
				glBindBufferBase(GL_UNIFORM_BUFFER, bindId, id);
			else
				glBindBuffer(static_cast<GLenum>(m_type), id);
		}

		void unbind()
		{
			ZoneScopedN("[OpenGL Buffer] unbind()");
			if (m_type == BufferType::CONSTANT_BUFFER)
				glBindBufferBase(GL_UNIFORM_BUFFER, bindId, 0);
			else
				glBindBuffer(static_cast<GLenum>(m_type), 0);
		}

		template<typename elementType>
		void bufferData(elementType* data, int size = 1)
		{
			ZoneScopedN("[OpenGL Buffer] bufferData()");

			if (size < 1)
			{
				size = m_size;
			}
			else if (size != m_size)
			{
				m_size = size;
				m_elementSize = sizeof(elementType);
				createBuffer(data);
				return;
			}

			if (m_type == BufferType::CONSTANT_BUFFER || m_usage == UsageType::IMMUTABLE)
			{
				ZoneScopedN("[OpenGL Buffer] [bufferData()] Buffering Immutable data");
				bind();
				glBufferSubData(static_cast<GLenum>(m_type), 0, m_size * sizeof(elementType), data);
			}
			else
			{
				ZoneScopedN("[OpenGL Buffer] [bufferData()] Buffering data");
				bind();
				glBufferData(static_cast<GLenum>(m_type), m_size * sizeof(elementType), data, static_cast<GLenum>(m_usage));
			}
		}

		void release()
		{
			ZoneScopedN("[OpenGL Buffer] release()");
			glDeleteBuffers(1, &id);
		}

	private:
		template<typename elementType>
		void createBuffer(elementType* data)
		{
			ZoneScopedN("[OpenGL Buffer] createBuffer()");
			if (id == 0)
			{
				ZoneScopedN("[OpenGL Buffer] [createBuffer()] Generate new Buffer");
				glGenBuffers(1, &id);
				bind();
			}

			if (m_usage == UsageType::IMMUTABLE)
			{
				ZoneScopedN("[OpenGL Buffer] [createBuffer()] Buffering Immutable data");
				glBufferStorage(static_cast<GLenum>(m_type), m_size * m_elementSize, data, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
			}
			else
			{
				ZoneScopedN("[OpenGL Buffer] [createBuffer()] Buffering the data");
				glBufferData(static_cast<GLenum>(m_type), m_size * m_elementSize, data, static_cast<GLenum>(m_usage));
				if (m_type == BufferType::CONSTANT_BUFFER)
				{
					ZoneScopedN("[OpenGL Buffer] [createBuffer()] Specialization for a Const Buffer");
					glBindBufferRange(static_cast<GLenum>(m_type), 0, id, 0, static_cast<unsigned int>(m_elementSize + (16 - (m_elementSize % 16))));
				}
			}
		}
	};
}