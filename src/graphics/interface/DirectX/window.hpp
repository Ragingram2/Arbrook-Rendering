#pragma once
#include <string>
#include <string_view>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <rsl/logging>

#include "core/utils/profiler.hpp"

#include "core/math/math.hpp"
#include "graphics/interface/DirectX/dx11includes.hpp"

namespace rythe::rendering::internal
{
	namespace log = rsl::log;
	struct window
	{
	private:
		GLFWwindow* m_glfwWindow;
		HWND m_hwnd;
	public:
		math::ivec2 m_resolution;
		std::string m_windowName;

		IDXGISwapChain* swapchain = nullptr;             // the pointer to the swap chain interface
		DXDevice dev = nullptr;                     // the pointer to our Direct3D device interface
		DXDeviceContext devcon = nullptr;           // the pointer to our Direct3D device context
		DXInfoQueue infoQueue = nullptr;
		DXGIInfoQueue dxgiInfoQueue = nullptr;
	public:
		window() = default;
		window(window& hwnd)
		{
			m_resolution = hwnd.m_resolution;
			m_windowName = hwnd.m_windowName;
			m_glfwWindow = hwnd.getGlfwWindow();

			swapchain = hwnd.swapchain;
			dev = hwnd.dev;
			devcon = hwnd.devcon;
			infoQueue = hwnd.infoQueue;
		}
		window operator=(const window& wind)
		{
			m_glfwWindow = wind.m_glfwWindow;
			m_hwnd = wind.m_hwnd;
			m_resolution = wind.m_resolution;
			m_windowName = wind.m_windowName;

			swapchain = wind.swapchain;
			dev = wind.dev;
			devcon = wind.devcon;
			infoQueue = wind.infoQueue;
			return *this;
		}

		window operator=(window&& wind)
		{
			m_glfwWindow = wind.m_glfwWindow;
			m_hwnd = wind.m_hwnd;
			m_resolution = wind.m_resolution;
			m_windowName = wind.m_windowName;

			swapchain = wind.swapchain;
			dev = wind.dev;
			devcon = wind.devcon;
			infoQueue = wind.infoQueue;
			return *this;
		}

		void initialize(math::ivec2 res, const std::string& name, GLFWwindow* window = nullptr)
		{
			ZoneScopedN("[DX11 Window] initialize()");
			m_resolution = res;
			m_windowName = name;
			if (!window)
				m_glfwWindow = glfwCreateWindow(res.x, res.y, name.c_str(), NULL, NULL);
			else
				m_glfwWindow = window;

			m_hwnd = glfwGetWin32Window(m_glfwWindow);
		}

		GLFWwindow* getGlfwWindow()
		{
			ZoneScopedN("[DX11 Window] getGlfwWindow()");
			return m_glfwWindow;
		}

		HWND getHWND()
		{
			ZoneScopedN("[DX11 Window] getHWND()");
			return m_hwnd;
		}

		void swapBuffers()
		{
			ZoneScopedN("[DX11 Window] swapBuffers()");
			glfwSwapBuffers(m_glfwWindow);
		}

		void setSwapInterval(int interval)
		{
			ZoneScopedN("[DX11 Window] setSwapInterval()");
			glfwSwapInterval(interval);
		}

		void pollEvents()
		{
			ZoneScopedN("[DX11 Window] pollEvents()");
			glfwPollEvents();
		}

		bool shouldClose()
		{
			ZoneScopedN("[DX11 Window] shouldClose()");
			return glfwWindowShouldClose(m_glfwWindow);
		}

		void setWindowTitle(const std::string& name)
		{
			ZoneScopedN("[DX11 Window] setWindowTitle()");
			glfwSetWindowTitle(m_glfwWindow, name.data());
		}

		void makeCurrent()
		{
			ZoneScopedN("[DX11 Window] makeCurrent()");
			glfwMakeContextCurrent(m_glfwWindow);
		}

		math::ivec2 getResolution() const
		{
			ZoneScopedN("[DX11 Window] getResolution()");
			return m_resolution;
		}

		std::string getName() const
		{
			ZoneScopedN("[DX11 Window] getName()");
			return m_windowName;
		}

		void checkError()
		{
			ZoneScopedN("[DX11 Window] checkError()");
#if _DEBUG

			//DX11 error checking
			UINT64 message_count = infoQueue->GetNumStoredMessages();
			for (UINT64 i = 0; i < message_count; i++) {
				SIZE_T message_size = 0;
				infoQueue->GetMessage(i, nullptr, &message_size);

				D3D11_MESSAGE* message = (D3D11_MESSAGE*)malloc(message_size);
				infoQueue->GetMessage(i, message, &message_size);
				switch (message->Severity)
				{
				case D3D11_MESSAGE_SEVERITY_CORRUPTION:
					rsl::log::error("DX11: {}", message->pDescription);
					break;
				case D3D11_MESSAGE_SEVERITY_ERROR:
					rsl::log::error("DX11: {}", message->pDescription);
					break;
				case D3D11_MESSAGE_SEVERITY_INFO:
				case D3D11_MESSAGE_SEVERITY_MESSAGE:
					rsl::log::info("DX11: {}", message->pDescription);
					break;
				case D3D11_MESSAGE_SEVERITY_WARNING:
					rsl::log::warn("DX11: {}", message->pDescription);
					break;
				}

				free(message);
			}
			infoQueue->ClearStoredMessages();

			DXGI_DEBUG_ID id = DXGI_DEBUG_DXGI;
			//DXGI Errors
			message_count = dxgiInfoQueue->GetNumStoredMessages(id);
			for (UINT64 i = 0; i < message_count; i++) {
				SIZE_T message_size = 0;
				dxgiInfoQueue->GetMessageW(id, i, nullptr, &message_size);
				DXGI_INFO_QUEUE_MESSAGE* message = (DXGI_INFO_QUEUE_MESSAGE*)malloc(message_size);
				dxgiInfoQueue->GetMessageW(id, i, message, &message_size);
				switch (message->Severity)
				{
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION:
					rsl::log::error("DXGI: {}", message->pDescription);
					break;
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR:
					rsl::log::error("DXGI: {}", message->pDescription);
					break;
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO:
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE:
					rsl::log::info("DXGI: {}", message->pDescription);
					break;
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING:
					rsl::log::warn("DXGI: {}", message->pDescription);
					break;
				}

				free(message);
			}
			dxgiInfoQueue->ClearStoredMessages(id);
#endif
		}
	};
}