#pragma once
#include "core/utils/profiler.hpp"

#include "core/assets/assethandle.hpp"
#include "graphics/data/bufferhandle.hpp"
#include "graphics/data/shaderhandle.hpp"
#include "graphics/interface/definitions/shader.hpp"
#include "graphics/interface/definitions/enumtypes.hpp"

namespace ast = rythe::core::assets;
namespace rythe::rendering
{
	template<typename APIType>
	struct Iinputlayout
	{
	private:
		APIType m_impl;
	public:
		void initialize(unsigned int numBuffers, shader_handle shader) { m_impl.initialize(numBuffers, shader); }
		void bind() { m_impl.bind(); }
		void unbind() { m_impl.unbind(); }
		void setAttributePtr(buffer_handle buf, const std::string& attribName, unsigned int index, FormatType components, unsigned int inputSlot, unsigned int stride, unsigned int offset, InputClass inputClass = InputClass::PER_VERTEX, unsigned int instanceStep = 0)
		{
			m_impl.setAttributePtr(buf, attribName, index, static_cast<internal::FormatType>(components), inputSlot, stride, offset, static_cast<internal::InputClass>(inputClass), instanceStep);
		}
		void submitAttributes() { m_impl.submitAttributes(); }
		void release() { m_impl.release(); }

		unsigned int getId() { return m_impl.id; }
		void setId(unsigned int id) { m_impl.id = id; }
	};
}