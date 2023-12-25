#pragma once
#include <string>
#include <string_view>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <rsl/logging>

#include "core/math/math.hpp"
#include "rendering/interface/DirectX/dx11includes.hpp"

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
		ID3D11Device* dev = nullptr;                     // the pointer to our Direct3D device interface
		ID3D11DeviceContext* devcon = nullptr;           // the pointer to our Direct3D device context
		ID3D11RenderTargetView* backbuffer = nullptr;    // global declaration
		ID3D11DepthStencilView* depthStencilView = nullptr;
		ID3D11Texture2D* depthStencilBuffer = nullptr;
		ID3D11InfoQueue* infoQueue = nullptr;
	public:
		window() = default;
		window(window& hwnd)
		{
			m_resolution = hwnd.m_resolution;
			m_windowName = hwnd.m_windowName;
			m_glfwWindow = hwnd.getGlfwWindow();
		}
		window(math::ivec2 res, const std::string& name) : m_resolution(res), m_windowName(name) { }

		void initialize(math::ivec2 res, const std::string& name, GLFWwindow* window = nullptr)
		{
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
			return m_glfwWindow;
		}

		HWND getHWND()
		{
			return m_hwnd;
		}

		void swapBuffers()
		{
			glfwSwapBuffers(m_glfwWindow);
		}

		void setSwapInterval(int interval)
		{
			glfwSwapInterval(interval);
		}

		void pollEvents()
		{
			glfwPollEvents();
		}

		bool shouldClose()
		{
			return glfwWindowShouldClose(m_glfwWindow);
		}

		void setWindowTitle(const std::string& name)
		{
			glfwSetWindowTitle(m_glfwWindow, name.data());
		}

		void makeCurrent()
		{
			glfwMakeContextCurrent(m_glfwWindow);
		}

		math::ivec2 getResolution()
		{
			return m_resolution;
		}

		void checkError()
		{
#if _DEBUG
			UINT64 message_count = infoQueue->GetNumStoredMessages();
			D3D11_MESSAGE_ID hide[]
			{
					 D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
					 D3D11_MESSAGE_ID_SETPRIVATEDATA_INVALIDFREEDATA
			};
			D3D11_INFO_QUEUE_FILTER filter;
			ZeroMemory(&filter, sizeof(filter));
			filter.DenyList.NumIDs = sizeof(hide);
			filter.DenyList.pIDList = hide;
			infoQueue->AddStorageFilterEntries(&filter);

			for (UINT64 i = 0; i < message_count; i++) {
				SIZE_T message_size = 0;
				infoQueue->GetMessage(i, nullptr, &message_size);

				D3D11_MESSAGE* message = (D3D11_MESSAGE*)malloc(message_size);
				infoQueue->GetMessage(i, message, &message_size);
				switch (message->Severity)
				{
				case D3D11_MESSAGE_SEVERITY_CORRUPTION:
					rsl::log::fatal("DX11: {}", message->pDescription);
					break;
				case D3D11_MESSAGE_SEVERITY_ERROR:
					rsl::log::error("DX11: {}", message->pDescription);
					break;
				case D3D11_MESSAGE_SEVERITY_INFO:
					rsl::log::info("DX11: {}", message->pDescription);
					break;
				case D3D11_MESSAGE_SEVERITY_WARNING:
					rsl::log::warn("DX11: {}", message->pDescription);
					break;
				}

				free(message);
			}
			infoQueue->ClearStoredMessages();
#endif
		}
	};
}