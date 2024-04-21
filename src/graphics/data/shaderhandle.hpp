#pragma once
#include "graphics/interface/ishader.hpp"
#include "graphics/interface/config.hpp"
#include Shader_HPP_PATH

namespace rythe::rendering
{
	struct shader_handle
	{
		Ishader<internal::shader>* m_data = nullptr;
		shader_handle() = default;
		shader_handle(Ishader<internal::shader>* data) : m_data(data) {}
		shader_handle(Ishader<internal::shader>& data) : m_data(&data) {}
		shader_handle(const shader_handle& handle) : m_data(handle.m_data) {}

		Ishader<internal::shader>* operator->() { return m_data; }
		operator Ishader<internal::shader>& () const { return *m_data; }
		operator bool() const { return m_data != nullptr; }

		bool operator == (shader_handle handle) { return m_data == handle.m_data; }
		bool operator != (shader_handle handle) { return !operator==(handle); }

		bool operator ==(Ishader<internal::shader>* data) { return m_data == data; }
		bool operator !=(Ishader<internal::shader>* data) { return !operator==(data); }
	};

}
