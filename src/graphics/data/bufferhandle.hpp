#pragma once
#include "graphics/interface/ibuffer.hpp"
#include "graphics/interface/config.hpp"
#include Buffer_HPP_PATH

namespace rythe::rendering
{
	struct buffer_handle
	{
		Ibuffer<internal::buffer>* m_data = nullptr;
		buffer_handle() = default;
		buffer_handle(Ibuffer<internal::buffer>* data) : m_data(data) {}
		buffer_handle(Ibuffer<internal::buffer>& data) : m_data(&data) {}
		buffer_handle(const buffer_handle& handle) : m_data(handle.m_data) {}

		Ibuffer<internal::buffer>* operator->() { return m_data; }
		operator Ibuffer<internal::buffer>& () const { return *m_data; }
		operator bool() const { return m_data != nullptr; }

		bool operator == (buffer_handle handle) { return m_data == handle.m_data; }
		bool operator != (buffer_handle handle) { return !operator==(handle); }

		bool operator ==(Ibuffer<internal::buffer>* data) { return m_data == data; }
		bool operator !=(Ibuffer<internal::buffer>* data) { return !operator==(data); }
	};
}