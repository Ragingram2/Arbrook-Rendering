#pragma once
#include <string>
#include <fstream>
#include <memory>
#include <stdio.h>

#include <rsl/logging>
#include <rsl/math>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <DirectXMath.h>

#include "graphics/cache/windowprovider.hpp"
#include "graphics/data/shadersource.hpp"
#include "graphics/interface/DirectX/dx11includes.hpp"
#include "graphics/interface/definitions/window.hpp"
#include "graphics/interface/definitions/framebuffer.hpp"
#include "graphics/interface/config.hpp"
#include Shader_HPP_PATH
#include Buffer_HPP_PATH


namespace rythe::rendering::internal
{
	class RenderInterface
	{
	private:
		ID3D11RasterizerState* m_rasterizerState = nullptr;
		ID3D11DepthStencilState* m_depthStencilState = nullptr;
		D3D11_VIEWPORT m_viewport;
		DXGI_SWAP_CHAIN_DESC m_swapChainDesc;
		D3D11_RASTERIZER_DESC m_rasterizerDesc;
		D3D11_DEPTH_STENCIL_DESC m_depthStencilDesc;
		float m_colorData[4];
		window_handle m_windowHandle;
	public:
		void setWindow(window_handle handle)
		{
			ZoneScopedN("[DX11 Renderinterface] setWindow()");
			m_windowHandle = handle;
			m_windowHandle->makeCurrent();
		}

		void initialize(math::ivec2 res, const std::string& name, GLFWwindow* window = nullptr)
		{
			ZoneScopedN("[DX11 Renderinterface] initialize()");
			log::info("Initializing DX11");
			if (!window && WindowProvider::get(name) == nullptr)
				m_windowHandle = WindowProvider::addWindow(name);
			else if (WindowProvider::get(name) != nullptr)
			{
				m_windowHandle = WindowProvider::get(name);
				WindowProvider::setActive(name);
				return;
			}
			WindowProvider::setActive(name);

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			m_windowHandle->initialize(res, name, window);
			m_windowHandle->makeCurrent();

			ZeroMemory(&m_swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

			m_swapChainDesc.BufferCount = 1;
			m_swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			m_swapChainDesc.BufferDesc.Width = res.x;
			m_swapChainDesc.BufferDesc.Height = res.y;
			m_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			m_swapChainDesc.OutputWindow = static_cast<HWND>(m_windowHandle->getHWND());
			m_swapChainDesc.SampleDesc.Count = 1;
			m_swapChainDesc.Windowed = TRUE;
			m_swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			m_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;

			UINT creationFlags = D3D11_CREATE_DEVICE_DEBUG;


			HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
				D3D_DRIVER_TYPE_HARDWARE,
				NULL,
				creationFlags,
				NULL,
				NULL,
				D3D11_SDK_VERSION,
				&m_swapChainDesc,
				&m_windowHandle->swapchain,
				&m_windowHandle->dev,
				NULL,
				&m_windowHandle->devcon);

#ifdef _DEBUG
			hr = m_windowHandle->dev->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&m_windowHandle->infoQueue);
			CHECKERROR(hr, "Retrieving the info queue failed", checkError());
#endif

			ID3D11RenderTargetView* renderTargetView;
			//getting the default color attachment
			ID3D11Texture2D* m_backBuffer;
			hr = m_windowHandle->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_backBuffer);
			CHECKERROR(hr, "Retrieving the backbuffer failed", checkError());

			//assigning the default color attachment
			hr = m_windowHandle->dev->CreateRenderTargetView(m_backBuffer, NULL, &renderTargetView);
			CHECKERROR(hr, "Creating a render target view failed", checkError());

			m_backBuffer->Release();

			D3D11_TEXTURE2D_DESC m_depthTexDesc;
			//creating a texture for depth attachment
			ZeroMemory(&m_depthTexDesc, sizeof(D3D11_TEXTURE2D_DESC));

			m_depthTexDesc.Width = res.x;
			m_depthTexDesc.Height = res.y;
			m_depthTexDesc.MipLevels = 1;
			m_depthTexDesc.ArraySize = 1;
			m_depthTexDesc.Format = static_cast<DXGI_FORMAT>(FormatType::D24_S8);
			m_depthTexDesc.SampleDesc = m_swapChainDesc.SampleDesc;
			m_depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
			m_depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			m_depthTexDesc.CPUAccessFlags = 0;
			m_depthTexDesc.MiscFlags = 0;

			ID3D11Texture2D* depthStencilBuffer;

			hr = m_windowHandle->dev->CreateTexture2D(&m_depthTexDesc, NULL, &depthStencilBuffer);
			CHECKERROR(hr, "Creating the depth texture failed", checkError());

			ZeroMemory(&m_depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

			setDepthFunction(DepthFuncs::LESS);
			updateDepthStencil();


			ID3D11DepthStencilView* depthStencilView;
			D3D11_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
			ZeroMemory(&m_depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

			m_depthStencilViewDesc.Format = m_depthTexDesc.Format;
			m_depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

			//assigning texture to depth attachment
			hr = m_windowHandle->dev->CreateDepthStencilView(depthStencilBuffer, &m_depthStencilViewDesc, &depthStencilView);
			CHECKERROR(hr, "Creating the depth stencil view failed", checkError());

			//binding attachments
			m_windowHandle->devcon->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

			ZeroMemory(&m_rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

			m_rasterizerDesc.AntialiasedLineEnable = FALSE;
			m_rasterizerDesc.CullMode = D3D11_CULL_BACK;
			m_rasterizerDesc.DepthBias = 0;
			m_rasterizerDesc.DepthBiasClamp = 0.0f;
			m_rasterizerDesc.DepthClipEnable = TRUE;
			m_rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			m_rasterizerDesc.FrontCounterClockwise = TRUE;
			m_rasterizerDesc.MultisampleEnable = FALSE;
			m_rasterizerDesc.ScissorEnable = FALSE;
			m_rasterizerDesc.SlopeScaledDepthBias = 0.0f;

			// Create the rasterizer state object.
			CHECKERROR(m_windowHandle->dev->CreateRasterizerState(&m_rasterizerDesc, &m_rasterizerState), "Creating rasterizer state failed", checkError());

			m_windowHandle->devcon->RSSetState(m_rasterizerState);

			setViewport(1, 0, 0, res.x, res.y, 0, 1);

		}

		void close()
		{
			ZoneScopedN("[DX11 Renderinterface] close()");
			log::debug("Something Called Close");
			m_windowHandle->swapchain->SetFullscreenState(FALSE, NULL);
			m_windowHandle->swapchain->Release();
			m_windowHandle->dev->Release();
			m_windowHandle->devcon->Release();
		}

		GLFWwindow* getGlfwWindow()
		{
			ZoneScopedN("[DX11 Renderinterface] getGlfwWindow()");
			return m_windowHandle->getGlfwWindow();
		}

		window_handle getWindowHandle()
		{
			ZoneScopedN("[DX11 Renderinterface] getWindowHandle()");
			return m_windowHandle;
		}

		void makeCurrent()
		{
			ZoneScopedN("[DX11 Renderinterface] makeCurrent()");
			m_windowHandle->makeCurrent();
		}

		void setSwapInterval(int interval)
		{
			ZoneScopedN("[DX11 Renderinterface] setSwapInterval()");
			m_windowHandle->setSwapInterval(interval);
		}

		bool shouldWindowClose()
		{
			ZoneScopedN("[DX11 Renderinterface] shouldWindowClose()");
			return m_windowHandle->shouldClose();
		}

		void setWindowTitle(const std::string& name)
		{
			ZoneScopedN("[DX11 Renderinterface] setWindowTitle()");
			m_windowHandle->setWindowTitle(name);
		}

		void pollEvents()
		{
			ZoneScopedN("[DX11 Renderinterface] pollEvents()");
			m_windowHandle->pollEvents();
		}

		void swapBuffers()
		{
			ZoneScopedN("[DX11 Renderinterface] swapBuffers()");
			m_windowHandle->swapBuffers();
			m_windowHandle->swapchain->Present(0, 0);
		}

		void drawArrays(PrimitiveType mode, unsigned int startVertex, unsigned int vertexCount)
		{
			ZoneScopedN("[DX11 Renderinterface] drawArrays()");
			m_windowHandle->devcon->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(mode));
			m_windowHandle->devcon->Draw(vertexCount, startVertex);
		}

		void drawArraysInstanced(PrimitiveType mode, unsigned int vertexCount, unsigned int instanceCount, unsigned int startVertex, unsigned int startInstance)
		{
			ZoneScopedN("[DX11 Renderinterface] drawArraysInstanced()");
			m_windowHandle->devcon->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(mode));
			m_windowHandle->devcon->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
		}

		void drawIndexed(PrimitiveType mode, unsigned int indexCount, unsigned int startIndex, unsigned int baseVertex)
		{
			ZoneScopedN("[DX11 Renderinterface] drawIndexed()");
			m_windowHandle->devcon->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(mode));
			m_windowHandle->devcon->DrawIndexed(indexCount, startIndex, baseVertex);
		}

		void drawIndexedInstanced(PrimitiveType mode, unsigned int indexCount, unsigned int instanceCount, unsigned int startIndex, unsigned int baseVertex, unsigned int startInstance)
		{
			ZoneScopedN("[DX11 Renderinterface] drawIndexedInstanced()");
			m_windowHandle->devcon->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(mode));
			m_windowHandle->devcon->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance);
		}

		void clear(bool clearColor, internal::DepthClearBit flags)
		{
			ZoneScopedN("[DX11 Renderinterface] clear()");
			ID3D11RenderTargetView* views = NULL;
			ID3D11DepthStencilView* depthStencilView = NULL;
			m_windowHandle->devcon->OMGetRenderTargets(1, &views, &depthStencilView);

			if ((flags == internal::DepthClearBit::DEPTH_STENCIL || flags == internal::DepthClearBit::DEPTH || flags == internal::DepthClearBit::STENCIL) && depthStencilView != NULL)
				m_windowHandle->devcon->ClearDepthStencilView(depthStencilView, static_cast<D3D11_CLEAR_FLAG>(flags), 1.0f, 0);
			if (clearColor && views != NULL)
				m_windowHandle->devcon->ClearRenderTargetView(views, m_colorData);
		}

		void setClearColor(math::vec4 color)
		{
			ZoneScopedN("[DX11 Renderinterface] setClearColor()");
			m_colorData[0] = color.r;
			m_colorData[1] = color.g;
			m_colorData[2] = color.b;
			m_colorData[3] = color.a;
		}

		void setViewport(float numViewPorts, float leftX, float leftY, float width, float height, float minDepth, float maxDepth)
		{
			ZoneScopedN("[DX11 Renderinterface] setViewport()");
			if (width == 0 && height == 0)
			{
				width = m_windowHandle->m_resolution.x;
				height = m_windowHandle->m_resolution.y;
			}

			ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));

			m_viewport.TopLeftX = leftX;
			m_viewport.TopLeftY = leftY;
			m_viewport.Width = width;
			m_viewport.Height = height;
			m_viewport.MinDepth = minDepth;
			m_viewport.MaxDepth = maxDepth;

			m_windowHandle->devcon->RSSetViewports(numViewPorts, &m_viewport);
		}

		void cullFace(CullMode mode = CullMode::NONE)
		{
			ZoneScopedN("[DX11 Renderinterface] cullFace()");
			m_rasterizerDesc.CullMode = static_cast<D3D11_CULL_MODE>(mode);

			// Create the rasterizer state object.
			CHECKERROR(m_windowHandle->dev->CreateRasterizerState(&m_rasterizerDesc, &m_rasterizerState), "Creating rasterizer state failed", checkError());

			m_windowHandle->devcon->RSSetState(m_rasterizerState);
		}

		void setWindOrder(WindOrder order)
		{
			ZoneScopedN("[DX11 Renderinterface] setWindOrder()");
			m_rasterizerDesc.FrontCounterClockwise = (order == WindOrder::CCW);

			CHECKERROR(m_windowHandle->dev->CreateRasterizerState(&m_rasterizerDesc, &m_rasterizerState), "Creating rasterizer state failed", checkError());

			m_windowHandle->devcon->RSSetState(m_rasterizerState);
		}

		void depthTest(bool enable)
		{
			ZoneScopedN("[DX11 Renderinterface] depthTest()");
			m_depthStencilDesc.DepthEnable = enable;
		}

		void depthWrite(bool enable)
		{
			ZoneScopedN("[DX11 Renderinterface] depthWrite()");
			if (enable)
				m_depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			else
				m_depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		}

		void setStencilMask(int mask)
		{
			ZoneScopedN("[DX11 Renderinterface] setStencilMask()");
			m_depthStencilDesc.StencilWriteMask = mask;
		}

		void setDepthFunction(internal::DepthFuncs function)
		{
			ZoneScopedN("[DX11 Renderinterface] setDepthFunction()");
			m_depthStencilDesc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(function);
		}

		void stencilTest(bool enable)
		{
			ZoneScopedN("[DX11 Renderinterface] stencilTest()");
			m_depthStencilDesc.StencilEnable = enable;
		}

		void setStencilOp(Face face, StencilOp fail, StencilOp  zfail, StencilOp  zpass)
		{
			ZoneScopedN("[DX11 Renderinterface] setStencilOp()");
			switch (face)
			{
			case Face::FRONT:
				m_depthStencilDesc.FrontFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(fail);
				m_depthStencilDesc.FrontFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(zfail);
				m_depthStencilDesc.FrontFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(zpass);
				m_depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				m_depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
				m_depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				break;
			case Face::BACK:
				m_depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				m_depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
				m_depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				m_depthStencilDesc.BackFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(fail);
				m_depthStencilDesc.BackFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(zfail);
				m_depthStencilDesc.BackFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(zpass);
				break;
			case Face::FRONT_BACK:
				m_depthStencilDesc.FrontFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(fail);
				m_depthStencilDesc.FrontFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(zfail);
				m_depthStencilDesc.FrontFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(zpass);
				m_depthStencilDesc.BackFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(fail);
				m_depthStencilDesc.BackFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(zfail);
				m_depthStencilDesc.BackFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(zpass);
				break;
			default:
				break;
			}
		}

		void setStencilFunction(Face face, DepthFuncs func, unsigned int ref, unsigned int mask)
		{
			ZoneScopedN("[DX11 Renderinterface] setStencilFunction()");
			m_depthStencilDesc.StencilReadMask = mask;
			switch (face)
			{
			case Face::FRONT:
				m_depthStencilDesc.FrontFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(func);
				m_depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
				break;
			case Face::BACK:
				m_depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER;
				m_depthStencilDesc.BackFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(func);
				break;
			case Face::FRONT_BACK:
				m_depthStencilDesc.FrontFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(func);
				m_depthStencilDesc.BackFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(func);
				break;
			default:
				break;
			}
		}

		void updateDepthStencil()
		{
			ZoneScopedN("[DX11 Renderinterface] updateDepthStencil()");
			CHECKERROR(m_windowHandle->dev->CreateDepthStencilState(&m_depthStencilDesc, &m_depthStencilState), "Creating the depth stencil state failed", checkError());

			m_windowHandle->devcon->OMSetDepthStencilState(m_depthStencilState, 1);
		}

		void checkError()
		{
			ZoneScopedN("[DX11 Renderinterface] checkError()");
			m_windowHandle->checkError();
		}
	};
}