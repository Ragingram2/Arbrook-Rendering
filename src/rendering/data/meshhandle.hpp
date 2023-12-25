#pragma once
#include "rendering/data/mesh.hpp"

namespace rythe::rendering
{
	struct mesh_handle
	{
		mesh* m_data = nullptr;
		mesh_handle() = default;
		mesh_handle(mesh* data) : m_data(data) {}
		mesh_handle(mesh& data) : m_data(&data) {}

		mesh* operator->() { return m_data; }
		operator mesh& () const { return *m_data; }

		bool operator ==(mesh* data) { return m_data == data; }
		bool operator !=(mesh* data) { return m_data != data; }
	};
}
