#pragma once
#include <memory>
#include <vector>
#include <string>

#include <rsl/logging>

#include "graphics/data/windowhandle.hpp"

namespace rythe::rendering
{
	namespace log = rsl::log;
	struct WindowProvider
	{
	public:
		static window_handle activeWindow;
	private:
		//static std::vector<std::unique_ptr<internal::window>> m_windows;
		static std::unordered_map<rsl::id_type, std::unique_ptr<internal::window>> m_windows;
		static std::unordered_map<rsl::id_type, std::string> m_names;

	public:
		static window_handle setActive(const std::string& windowName)
		{
			auto id = rsl::nameHash(windowName);
			if (m_windows.count(id))
			{
				return activeWindow = window_handle{ m_windows[id].get() };
			}
			log::error("The window you tried to make active \"{}\", does not exist");
			return activeWindow;
		}

		static window_handle addWindow(const std::string& windowName)
		{
			auto id = rsl::nameHash(windowName);
			if (m_windows.count(id))
			{
				log::warn("A window with the name \"{}\", already exists. returning the existing handle", windowName);
				return { m_windows[id].get() };
			}

			m_names.emplace(id, windowName);
			auto& wind = m_windows.emplace(id, std::make_unique<internal::window>()).first->second;
			return { wind.get() };
		}

		static window_handle get(const std::string& windowName)
		{
			auto id = rsl::nameHash(windowName);
			if (m_windows.count(id))
			{
				return { m_windows[id].get() };
			}
			log::warn("The window \"{}\" does not exist", windowName);
			return { nullptr };
		}

		static void destroyWindow(const std::string& windowName)
		{
			auto id = rsl::nameHash(windowName);
			if (m_windows.count(id))
			{
				if (m_windows.size() < 2)
					log::debug("About to destroy last window");

				glfwDestroyWindow(m_windows[id].get()->getGlfwWindow());
				m_windows.erase(id);
				m_names.erase(id);
			}
		}
	};
	inline std::unordered_map<rsl::id_type, std::unique_ptr<internal::window>> WindowProvider::m_windows;
	inline std::unordered_map<rsl::id_type, std::string> WindowProvider::m_names;
	inline window_handle WindowProvider::activeWindow;
}