#pragma once
#include "rendering/data/model.hpp"

namespace rythe::rendering
{
	struct model_handle
	{
		model* m_data = nullptr;
		model_handle() = default;
		model_handle(model* data) : m_data(data) {}
		model_handle(model& data) : m_data(&data) {}

		model* operator->() { return m_data; }
		operator model& () const { return *m_data; }

		bool operator ==(model_handle handle) { return m_data == handle.m_data; }
		bool operator !=(model_handle handle) { return m_data != handle.m_data; }

		bool operator ==(model* data) { return m_data == data; }
		bool operator !=(model* data) { return m_data != data; }
	};
}