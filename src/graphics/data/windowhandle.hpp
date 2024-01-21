#pragma once
#include "graphics/interface/config.hpp"
#include Window_HPP_PATH

namespace rythe::rendering
{
	struct window_handle
	{
		internal::window* m_data = nullptr;
		window_handle() = default;
		window_handle(window_handle& handle) : m_data(handle.m_data) {}
		window_handle(internal::window* data) : m_data(data) {}
		window_handle(internal::window& data) : m_data(&data) {}

		internal::window* operator->() { return m_data; }
		operator internal::window& () const { return *m_data; }
		window_handle operator=(const window_handle& handle) { m_data = handle.m_data; return *this; }
		window_handle operator=(window_handle&& handle) { m_data = handle.m_data; return *this; }
		window_handle operator=(internal::window* window) { m_data = window; return *this; }

		bool operator ==(window_handle handle) { return m_data == handle.m_data; }
		bool operator !=(window_handle handle) { return !operator==(handle); }

		bool operator ==(internal::window* data) { return m_data == data; }
		bool operator !=(internal::window* data) { return !operator==(data); }
	};

}