#pragma once
#include <string>

#include "rendering/interface/definitions/enumtypes.hpp"

namespace rythe::rendering
{
	struct buffer_handle;
	class ShaderCache;
	namespace internal
	{
		struct inputlayout;
	}

	namespace internal
	{
		struct inputlayout;
	}

	template<typename APIType>
	struct Ishader
	{
		friend class ShaderCache;
		friend struct internal::inputlayout;
	private:
		APIType m_impl;
	public:
		void bind() { m_impl.bind(); }
		void addBuffer(buffer_handle handle) { m_impl.addBuffer(handle); }
		template<typename elementType>
		void setData(const std::string& bufferName, elementType data[]) { m_impl.setData(bufferName, data); }
		void release() { m_impl.release(); }
		void clearBuffers() { m_impl.clearBuffers(); }

		std::string getName() { return m_impl.name; }
		unsigned int getId() { return m_impl.programId; }
	private:
		APIType& getImpl() { return m_impl; }
	};
}