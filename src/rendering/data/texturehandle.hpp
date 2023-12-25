#pragma once
#include "rendering/interface/itexture.hpp"
#include "rendering/interface/config.hpp"
#include Texture_HPP_PATH

namespace rythe::rendering
{
	struct texture_handle
	{
		Itexture<internal::texture>* m_data = nullptr;
		texture_handle() = default;
		texture_handle(std::nullptr_t null_ptr) : m_data(null_ptr) {}
		texture_handle(Itexture<internal::texture>* data) : m_data(data) {}
		texture_handle(Itexture<internal::texture>& data) : m_data(&data) {}

		Itexture<internal::texture>* operator->() { return m_data; }
		operator Itexture<internal::texture>& () const { return *m_data; }

		bool operator ==(std::nullptr_t null_ptr) { return m_data == nullptr; }
		bool operator !=(std::nullptr_t null_ptr) { return m_data != nullptr; }
	};
}