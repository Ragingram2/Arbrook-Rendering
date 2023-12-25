#pragma once
#include <D3D11.h>
#include <D3Dcommon.h>

namespace rythe::rendering::internal
{
	enum class ShaderType
	{
		VERTEX = 0,
		FRAGMENT = 1,
		GEOMETRY = 2,
		HULL = 3,
		COMPUTE = 4
	};

	enum class TargetType
	{
		VERTEX_BUFFER = D3D11_BIND_VERTEX_BUFFER,
		INDEX_BUFFER = D3D11_BIND_INDEX_BUFFER,
		CONSTANT_BUFFER = D3D11_BIND_CONSTANT_BUFFER,
		TEXTURE2D = D3D11_BIND_SHADER_RESOURCE
	};

	enum class UsageType
	{
		DEFAULT = D3D11_USAGE_DEFAULT,
		IMMUTABLE = D3D11_USAGE_IMMUTABLE,
		STATICDRAW = D3D11_USAGE_DYNAMIC,
		STAGING = D3D11_USAGE_STAGING
	};

	enum class PrimitiveType
	{
		TRIANGLESLIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		TRIANGLESTRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		LINELIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		LINESTRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		POINTLIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
	};

	enum class DataType
	{
		INT = sizeof(int),
		UINT = sizeof(unsigned int),
		BYTE = sizeof(char),
		UBYTE = sizeof(unsigned char),
		SHORT = sizeof(short int),
		USHORT = sizeof(unsigned short int),
		FLOAT = sizeof(float)
	};

	enum class FormatType 
	{
		D24_S8 = DXGI_FORMAT_D24_UNORM_S8_UINT,
		RGBA32F = DXGI_FORMAT_R32G32B32A32_FLOAT,
		RGB32F = DXGI_FORMAT_R32G32B32_FLOAT,
		RG32F = DXGI_FORMAT_R32G32_FLOAT,
		R32F = DXGI_FORMAT_R32_FLOAT,
		R32U = DXGI_FORMAT_R32_UINT,
		RGBA = DXGI_FORMAT_R8G8B8A8_UNORM,
		RGB = DXGI_FORMAT_R8G8B8A8_UNORM,
		RG = DXGI_FORMAT_R8G8_UNORM,
		R = DXGI_FORMAT_R8_UNORM,
		BGRA = DXGI_FORMAT_B8G8R8A8_UNORM
	};

	enum class WrapMode
	{
		CLAMP = D3D11_TEXTURE_ADDRESS_CLAMP,
		CLAMP_TO_BORDER = D3D11_TEXTURE_ADDRESS_BORDER,
		REPEAT = D3D11_TEXTURE_ADDRESS_MIRROR
	};

	enum class FilterMode
	{
		NEAREST = D3D11_FILTER_MIN_MAG_MIP_POINT,
		LINEAR = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
		NEAREST_MIPMAP_NEAREST = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
		NEAREST_MIPMAP_LINEAR = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,
		LINEAR_MIPMAP_NEAREST = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,
		LINEAR_MIPMAP_LINEAR = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR
	};

	enum class ClearBit
	{
		COLOR = 0,
		DEPTH = D3D11_CLEAR_DEPTH,
		STENCIL = D3D11_CLEAR_STENCIL,
		COLOR_DEPTH = D3D11_CLEAR_DEPTH,
		DEPTH_STENCIL = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		COLOR_DEPTH_STENCIL = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL
	};

	enum class DepthFuncs
	{
		ALWAYS = D3D11_COMPARISON_ALWAYS,
		NEVER = D3D11_COMPARISON_NEVER,
		LESS = D3D11_COMPARISON_LESS,
		EQUAL = D3D11_COMPARISON_EQUAL,
		LESS_EQUAL = D3D11_COMPARISON_LESS_EQUAL,
		GREATER = D3D11_COMPARISON_GREATER,
		NOT_EQUAL = D3D11_COMPARISON_NOT_EQUAL,
		GREATER_EQUAL = D3D11_COMPARISON_GREATER_EQUAL
	};

	enum class StencilOp
	{
		KEEP = D3D11_STENCIL_OP_KEEP,
		ZERO = D3D11_STENCIL_OP_ZERO,
		REPLACE = D3D11_STENCIL_OP_REPLACE,
		INCR = D3D11_STENCIL_OP_INCR,
		INCR_WRAP = D3D11_STENCIL_OP_INCR_SAT,
		DECR = D3D11_STENCIL_OP_DECR,
		DECR_WRAP = D3D11_STENCIL_OP_DECR_SAT,
		INVERT = D3D11_STENCIL_OP_INVERT
	};

	enum class CullMode
	{
		FRONT = D3D11_CULL_FRONT,
		BACK = D3D11_CULL_BACK,
		NONE = D3D11_CULL_NONE
	};

	enum class Face
	{
		FRONT = 0,
		BACK = 1,
		FRONT_BACK = 2,
		NONE = -1
	};

	enum class WindOrder
	{
		CW = false,
		CCW = true
	};

	enum class InputClass
	{
		PER_VERTEX = 0,
		PER_INSTANCE = 1
	};

	enum class TextureSlot
	{
		TEXTURE0 = 0,
		TEXTURE1 = 1,
		TEXTURE2 = 2,
		TEXTURE3 = 3,
		TEXTURE4 = 4,
		TEXTURE5 = 5,
		TEXTURE6 = 6,
		TEXTURE7 = 7,
		TEXTURE8 = 8,
		TEXTURE9 = 9,
		TEXTURE10 = 10,
		TEXTURE11 = 11,
		TEXTURE12 = 12,
		TEXTURE13 = 13,
		TEXTURE14 = 14,
		TEXTURE15 = 15,
		TEXTURE16 = 16,
		TEXTURE17 = 17,
		TEXTURE18 = 18,
		TEXTURE19 = 19,
		TEXTURE20 = 20,
		TEXTURE21 = 21,
		TEXTURE22 = 22,
		TEXTURE23 = 23,
		TEXTURE24 = 24,
		TEXTURE25 = 25,
		TEXTURE26 = 26,
		TEXTURE27 = 27,
		TEXTURE28 = 28,
		TEXTURE29 = 29,
		TEXTURE30 = 30,
		TEXTURE31 = 31
	};
}