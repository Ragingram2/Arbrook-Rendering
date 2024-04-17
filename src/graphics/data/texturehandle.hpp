#pragma once
#include "graphics/interface/itexture.hpp"
#include "graphics/interface/config.hpp"
#include Texture_HPP_PATH

namespace rythe::rendering
{
	struct texture_handle
	{
		Itexture<internal::texture>* m_data = nullptr;
		texture_handle() = default;
		texture_handle(Itexture<internal::texture>* data) : m_data(data) {}
		texture_handle(Itexture<internal::texture>& data) : m_data(&data) {}
		texture_handle(const texture_handle& handle) : m_data(handle.m_data) {}

		Itexture<internal::texture>* operator->() { return m_data; }
		operator Itexture<internal::texture>& () const { return *m_data; }
		operator bool() const { return m_data != nullptr; }

		bool operator == (texture_handle handle) { return m_data == handle.m_data; }
		bool operator != (texture_handle handle) { return !operator==(handle); }

		bool operator ==(Itexture<internal::texture>* data) { return m_data == data; }
		bool operator !=(Itexture<internal::texture>* data) { return !operator==(data); }
	};
}