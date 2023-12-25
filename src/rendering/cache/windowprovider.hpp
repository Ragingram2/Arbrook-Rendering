#pragma once
#include <memory>
#include <vector>
#include <string>

#include "rendering/data/windowhandle.hpp"

namespace rythe::rendering
{
	struct WindowProvider
	{
	private:
		static std::vector<std::unique_ptr<internal::window>> m_windows;
	public:
		static window_handle addWindow()
		{
			auto& wind = m_windows.emplace_back(std::make_unique<internal::window>());
			return { wind.get() };
		}

		static window_handle get(int index)
		{
			if (index <= m_windows.size() - 1)
			{
				return { m_windows[index].get() };
			}
			return { nullptr };
		}
	};
	inline std::vector<std::unique_ptr<internal::window>> WindowProvider::m_windows;
}