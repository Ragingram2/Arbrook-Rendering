#pragma once
#include <vector>
#include <concepts>

#include <rsl/primitives>

#include "rendering/interface/definitions/enumtypes.hpp"

namespace rythe::rendering
{
	class BufferCache;
	namespace internal
	{
		struct shader;
		struct window;
	}
}

//This is where my addition notes will go
//Support owning multiple buffers
//possibly move currently bound buffers to the render interface

namespace rythe::rendering
{
	template<typename APIType>
	struct Ibuffer
	{
		friend class BufferCache;
		friend struct internal::shader;
	private:
		APIType m_impl;
	public:
		template<typename elementType>
		void initialize(TargetType target, UsageType usage, int size, elementType data[] = nullptr) { m_impl.template initialize<elementType>(static_cast<internal::TargetType>(target), static_cast<internal::UsageType>(usage), size, data); }
		void bind() { m_impl.bind(); }
		template<typename elementType>
		void bufferData(elementType data[], int size = 1) { m_impl.bufferData(data, size); }
		void release() { m_impl.release(); }

		std::string getName() { return m_impl.name; }
		void setName(std::string name) { m_impl.name = name; }
		unsigned int getId() { return m_impl.id; }
		void setId(unsigned int id) { m_impl.id = id; }
		TargetType getTargetType() { return static_cast<TargetType>(m_impl.m_target); }
	private:
		APIType& getImpl() { return m_impl; }
	};
}