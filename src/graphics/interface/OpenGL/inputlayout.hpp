#pragma once
#include <memory>
#include <set>
#include <algorithm>


#include <rsl/primitives>
#include <rsl/logging>

#include "core/assets/assethandle.hpp"
#include "graphics/data/shaderhandle.hpp"
#include "graphics/data/vertexattribute.hpp"
#include "graphics/interface/definitions/shader.hpp"
#include "graphics/interface/OpenGL/oglincludes.hpp"
#include "graphics/interface/config.hpp"
#include Buffer_HPP_PATH
#include EnumTypes_HPP_PATH

namespace ast = rythe::core::assets;
namespace rythe::rendering::internal
{
	namespace log = rsl::log;
	struct inputlayout
	{
	public:
		unsigned int id;
	private:
		std::unordered_map<unsigned int, std::vector<vertexattribute>> m_vertexAttribs;
		std::unordered_map<int, std::set<int>> m_flatIndeces;
	public:

		void initialize(unsigned int numBuffers, shader_handle shader)
		{
			ZoneScopedN("[OpenGL Inputlayout] initialize()");
			glGenVertexArrays(1, &id);
		}

		void bind()
		{
			ZoneScopedN("[OpenGL Inputlayout] bind()");
			glBindVertexArray(id);
		}

		void unbind()
		{
			ZoneScopedN("[OpenGL Inputlayout] unbind()");
			glBindVertexArray(0);
		}

		void setAttributePtr(buffer_handle buf, const std::string& attribName, unsigned int index, FormatType components, unsigned int inputSlot, unsigned int stride, unsigned int offset, InputClass inputClass, unsigned int instancedStep)
		{
			ZoneScopedN("[OpenGL Inputlayout] setAttributePtr()");
			m_vertexAttribs[buf->getId()].emplace_back(vertexattribute{ attribName.c_str(), index, components, inputSlot, stride, offset, inputClass, instancedStep });
		}

		void submitAttributes()
		{
			ZoneScopedN("[OpenGL Inputlayout] submitAttributes()");
			bind();
			if (m_vertexAttribs.size() > 0)
			{
				for (auto& [bufId, attribs] : m_vertexAttribs)
				{
					ZoneScopedN("[OpenGL Inputlayout][submitAttributes()] iterating over buffers attributes");
					for (auto& attrib : attribs)
					{
						ZoneScopedN("[OpenGL Inputlayout][submitAttributes()] iterating over attributes");
						glBindBuffer(GL_ARRAY_BUFFER, bufId);
						switch (attrib.format)
						{
						case FormatType::RGB32F:
							glVertexAttribPointer(attrib.inputSlot, 3, static_cast<GLenum>(DataType::FLOAT), false, attrib.stride, reinterpret_cast<void*>(attrib.offset));
							break;
						case FormatType::RGBA32F:
							glVertexAttribPointer(attrib.inputSlot, 4, static_cast<GLenum>(DataType::FLOAT), false, attrib.stride, reinterpret_cast<void*>(attrib.offset));
							break;
						case FormatType::R32U:
							glVertexAttribPointer(attrib.inputSlot, 1, static_cast<GLenum>(DataType::UINT), false, attrib.stride, reinterpret_cast<void*>(attrib.offset));
							break;
						case FormatType::RG32F:
							glVertexAttribPointer(attrib.inputSlot, 2, static_cast<GLenum>(DataType::FLOAT), false, attrib.stride, reinterpret_cast<void*>(attrib.offset));
							break;
						default:
							log::warn("Format is not supported for vertex attributes");
							continue;
							break;
						}

						glEnableVertexAttribArray(attrib.inputSlot);
						switch (attrib.inputClass)
						{
						case InputClass::PER_VERTEX:
							glVertexAttribDivisor(attrib.inputSlot, 0);
							break;
						case InputClass::PER_INSTANCE:
							glVertexAttribDivisor(attrib.inputSlot, attrib.step);
							break;
						}
						glBindBuffer(GL_ARRAY_BUFFER, 0);
					}
				}
				clearAttributes();
			}
			unbind();
		}

		void clearAttributes()
		{
			ZoneScopedN("[OpenGL Inputlayout] clearAttributes()");
			m_vertexAttribs.clear();
		}

		void release()
		{
			ZoneScopedN("[OpenGL Inputlayout] release()");
			glDeleteVertexArrays(1, &id);
			clearAttributes();
		}
	};
}