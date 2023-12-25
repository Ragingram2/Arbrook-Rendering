#pragma once
#include "rendering/data/material.hpp"

namespace rythe::rendering
{
	struct material_handle
	{
		material* m_data = nullptr;
		material_handle() = default;
		material_handle(material* data) : m_data(data) {}
		material_handle(material& data) : m_data(&data) {}

		material* operator->() { return m_data; }
		operator material& () const { return *m_data; }

		bool operator ==(material_handle handle) { return m_data == handle.m_data; }
		bool operator !=(material_handle handle) { return m_data != handle.m_data; }

		bool operator ==(material* data) { return m_data == data; }
		bool operator !=(material* data) { return m_data != data; }
	};
}