#pragma once
#include <wrl\client.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <D3Dcompiler.h>
#include <DxErr.h>
#include <dxcerrors.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>


#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")
#pragma comment (lib, "DxErr.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")

typedef Microsoft::WRL::ComPtr<IDXGISwapChain> DXSwapChain;
typedef Microsoft::WRL::ComPtr<ID3D11Device> DXDevice;
typedef Microsoft::WRL::ComPtr<ID3D11DeviceContext> DXDeviceContext;
typedef Microsoft::WRL::ComPtr<ID3D11InfoQueue> DXInfoQueue;
typedef Microsoft::WRL::ComPtr<IDXGIInfoQueue> DXGIInfoQueue;

typedef Microsoft::WRL::ComPtr<ID3D11RasterizerState> DXRasterizerState;
typedef Microsoft::WRL::ComPtr<ID3D11InputLayout> DXInputLayout;
typedef Microsoft::WRL::ComPtr<ID3D11VertexShader> DXVertexShader;
typedef Microsoft::WRL::ComPtr<ID3D11PixelShader> DXPixelShader;
typedef Microsoft::WRL::ComPtr<ID3D11GeometryShader> DXGeometryShader;
typedef Microsoft::WRL::ComPtr<ID3D11HullShader> DXHullShader;
typedef Microsoft::WRL::ComPtr<ID3D11ComputeShader> DXComputeShader;
typedef Microsoft::WRL::ComPtr<ID3DBlob> DXBlob;
typedef Microsoft::WRL::ComPtr<ID3D11Buffer> DXBuffer;
typedef Microsoft::WRL::ComPtr<ID3D11Texture2D> DXTexture2D;
typedef Microsoft::WRL::ComPtr<ID3D11SamplerState> DXSamplerState;
typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DXDepthStencilView;
typedef Microsoft::WRL::ComPtr<ID3D11RenderTargetView> DXRenderTargetView;
typedef Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> DXShaderResourceView;



