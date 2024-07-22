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
#include "graphics/cache/texturecache.hpp"
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
		DXGI_SWAP_CHAIN_DESC1 m_swapChainDesc;
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

			HRESULT hr;
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

			if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&m_windowHandle->dxgiFactory))))
			{
				rsl::log::error("DXGI: Unable to create DXGIFactory");
				return;
			}
			UINT deviceFlags = D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
			deviceFlags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif
			constexpr D3D_FEATURE_LEVEL deviceFeatureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
			if (FAILED(D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				deviceFlags,
				&deviceFeatureLevel,
				1,
				D3D11_SDK_VERSION,
				&m_windowHandle->dev,
				nullptr,
				&m_windowHandle->devcon)))
			{
				rsl::log::error("D3D11: Failed to create device and device Context");
				return;
			}

			ZeroMemory(&m_swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));

			m_swapChainDesc.Width = res.x;
			m_swapChainDesc.Height = res.y;
			m_swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			m_swapChainDesc.SampleDesc.Count = 1;
			m_swapChainDesc.SampleDesc.Quality = 0;
			m_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			m_swapChainDesc.BufferCount = 2;
			m_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
			m_swapChainDesc.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
			m_swapChainDesc.Flags = {};

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDescriptor = {};
			swapChainFullscreenDescriptor.Windowed = true;
			swapChainFullscreenDescriptor.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_CENTERED;

			if (FAILED(m_windowHandle->dxgiFactory->CreateSwapChainForHwnd(
				m_windowHandle->dev.Get(),
				static_cast<HWND>(m_windowHandle->getHWND()),
				&m_swapChainDesc,
				&swapChainFullscreenDescriptor,
				nullptr,
				m_windowHandle->swapchain.GetAddressOf())))
			{
				rsl::log::error("DXGI: Failed to create swapchain");
				return;
			}

#ifdef _DEBUG
			hr = m_windowHandle->dev->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&m_windowHandle->infoQueue);
			CHECKERROR(hr, "DXD11: Retrieving the info queue failed", checkError());

			if (FAILED(m_windowHandle->dev.As(&m_windowHandle->debug)))
			{
				rsl::log::error("D3D11: Failed to get the debug layer from the device");
			}

			Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;

			typedef HRESULT(WINAPI* LPDXGIGETDEBUGINTERFACE)(REFIID, void**);

			HMODULE dxgidebug = LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
			if (dxgidebug)
			{
				auto dxgiGetDebugInterface = reinterpret_cast<LPDXGIGETDEBUGINTERFACE>(
					reinterpret_cast<void*>(GetProcAddress(dxgidebug, "DXGIGetDebugInterface")));

				if (SUCCEEDED(dxgiGetDebugInterface(IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
				{
					dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
					dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
				}
			}
#endif

			//getting the default color attachment
			DXTexture2D m_backBuffer;

			if (FAILED(m_windowHandle->swapchain->GetBuffer(0, IID_PPV_ARGS(m_backBuffer.GetAddressOf()))))
			{
				std::cout << "D3D11: Failed to get Back Buffer from the SwapChain\n";
				checkError();
			}
			//assigning the default color attachment

			if (FAILED(m_windowHandle->dev->CreateRenderTargetView(
				m_backBuffer.Get(),
				nullptr,
				m_windowHandle->renderTargetView.GetAddressOf())))
			{
				std::cout << "D3D11: Failed to create RTV from Back Buffer\n";
				checkError();
			}

			m_backBuffer.Reset();
			m_windowHandle->devcon->ClearState();
			m_windowHandle->devcon->Flush();

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


			hr = m_windowHandle->dev->CreateTexture2D(&m_depthTexDesc, NULL, m_windowHandle->depthStencil.GetAddressOf());
			CHECKERROR(hr, "Creating the depth texture failed", checkError());

			ZeroMemory(&m_depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

			setDepthFunction(DepthFuncs::LESS);
			updateDepthStencil();


			D3D11_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
			ZeroMemory(&m_depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

			m_depthStencilViewDesc.Format = m_depthTexDesc.Format;
			m_depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

			//assigning texture to depth attachment
			hr = m_windowHandle->dev->CreateDepthStencilView(m_windowHandle->depthStencil.Get(), &m_depthStencilViewDesc, m_windowHandle->depthStencilView.GetAddressOf());
			log::debug("Creating Default DepthStencil View \"{}\"", static_cast<void*>(m_windowHandle->depthStencilView.Get()));
			CHECKERROR(hr, "Creating the depth stencil view failed", checkError());

			//binding attachments
			m_windowHandle->devcon->OMSetRenderTargets(1, &m_windowHandle->renderTargetView, m_windowHandle->depthStencilView.Get());

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

		void flush()
		{
			m_windowHandle->devcon->ClearState();
			m_windowHandle->devcon->Flush();
		}

		void resize(int width, int height)
		{
			DXTexture2D m_backBuffer;
			if (FAILED(m_windowHandle->swapchain->GetBuffer(0, IID_PPV_ARGS(&m_backBuffer))))
			{
				std::cout << "D3D11: Failed to get Back Buffer from the SwapChain\n";
				checkError();
			}

			ID3D11RenderTargetView* nullViews[] = { nullptr };
			m_windowHandle->devcon->OMSetRenderTargets(0, nullViews, nullptr);

			m_windowHandle->depthStencilView.Reset();
			m_windowHandle->depthStencil.Reset();
			m_windowHandle->renderTargetView.Reset();
			m_backBuffer.Reset();

			m_windowHandle->devcon->ClearState();
			m_windowHandle->devcon->Flush();


			CHECKERROR(m_windowHandle->swapchain->ResizeBuffers(
				0,
				width,
				height,
				DXGI_FORMAT_UNKNOWN,
				0u
			), "Failed Swapchain Resize", checkError();	m_windowHandle->debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL));

			m_windowHandle->swapchain->GetBuffer(0, IID_PPV_ARGS(m_backBuffer.GetAddressOf()));

			CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_UNKNOWN);
			m_windowHandle->dev->CreateRenderTargetView(
				m_backBuffer.Get(),
				&renderTargetViewDesc,
				m_windowHandle->renderTargetView.GetAddressOf()
			);

			setViewport(1, 0, 0, width, height, 0, 1);
		}

		void clear(bool clearColor, internal::DepthClearBit flags)
		{
			ZoneScopedN("[DX11 Renderinterface] clear()");
			DXRenderTargetView rtvs = nullptr;
			DXDepthStencilView depthStencilView = nullptr;
			m_windowHandle->devcon->OMGetRenderTargets(1, &rtvs, &depthStencilView);

			if ((flags == internal::DepthClearBit::DEPTH_STENCIL || flags == internal::DepthClearBit::DEPTH || flags == internal::DepthClearBit::STENCIL) && depthStencilView != NULL)
				m_windowHandle->devcon->ClearDepthStencilView(depthStencilView.Get(), static_cast<D3D11_CLEAR_FLAG>(flags), 1.0f, 0);
			if (clearColor && rtvs != NULL)
				m_windowHandle->devcon->ClearRenderTargetView(rtvs.Get(), m_colorData);

			depthStencilView.Reset();
			rtvs.Reset();
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

		math::vec4 readPixel(rendering::framebuffer& fbo, int x, int y, int width, int height)
		{
			texture_handle handle = fbo.getAttachment(rendering::AttachmentSlot::COLOR0);
			D3D11_TEXTURE2D_DESC desc;
			desc = handle->m_impl.m_texDesc;
			desc.Usage = D3D11_USAGE_STAGING;
			desc.BindFlags = 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.MiscFlags = 0;
			DXTexture2D stagingTexture;
			HRESULT hr = m_windowHandle->dev->CreateTexture2D(&desc, nullptr, &stagingTexture);
			if (FAILED(hr))
			{
				log::error("Staging texture creation failed");
				__debugbreak();
			}

			D3D11_BOX srcBox;
			srcBox.left = x;
			srcBox.right = srcBox.left + width;
			srcBox.top = y;
			srcBox.bottom = srcBox.top + height;
			srcBox.front = 0;
			srcBox.back = 1;

			m_windowHandle->devcon->CopySubresourceRegion(stagingTexture.Get(), 0, 0, 0, 0, handle->m_impl.m_texture.Get(), NULL, &srcBox);

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			m_windowHandle->devcon->Map(stagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
			auto pixel = static_cast<rsl::uint32*>(mappedResource.pData);
			m_windowHandle->devcon->Unmap(stagingTexture.Get(), 0);
			stagingTexture.Reset();
			return math::vec4(pixel[0], pixel[1], pixel[2], pixel[3]);
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