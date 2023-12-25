#pragma once
#include "rendering/interface/definitions/shader.hpp"

namespace rythe::rendering
{
	struct shader_handle
	{
		Ishader<internal::shader>* m_data = nullptr;
		shader_handle() = default;
		shader_handle(Ishader<internal::shader>* data) : m_data(data) {}
		shader_handle(Ishader<internal::shader>& data) : m_data(&data) {}

		Ishader<internal::shader>* operator->() { return m_data; }
		operator Ishader<internal::shader >& () const { return *m_data; }

		bool operator == (shader_handle handle) { return m_data == handle.m_data; }
		bool operator != (shader_handle handle) { return m_data != handle.m_data; }

		bool operator ==(Ishader<internal::shader>* data) { return m_data == data; }
		bool operator !=(Ishader<internal::shader>* data) { return m_data != data; }
	};
}