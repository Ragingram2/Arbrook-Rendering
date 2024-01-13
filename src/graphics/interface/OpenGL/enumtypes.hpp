#pragma once
#include "graphics/interface/OpenGL/oglincludes.hpp"

namespace rythe::rendering::internal
{
	enum class ShaderType
	{
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		GEOMETRY = GL_GEOMETRY_SHADER,
		HULL = GL_TESS_EVALUATION_SHADER,
		//DOMAIN = GL_TESS_CONTROL_SHADER,
		COMPUTE = GL_COMPUTE_SHADER
	};

	enum class TargetType : GLenum
	{
		VERTEX_BUFFER = GL_ARRAY_BUFFER,
		INDEX_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
		CONSTANT_BUFFER = GL_UNIFORM_BUFFER,
		TEXTURE2D = GL_TEXTURE_2D
	};

	enum class UsageType
	{
		DEFAULT = GL_DYNAMIC_DRAW,
		IMMUTABLE = 0,
		STATICDRAW = GL_STATIC_DRAW,
		STAGING = GL_STREAM_DRAW
	};

	enum class PrimitiveType
	{
		TRIANGLESLIST = GL_TRIANGLES,
		TRIANGLESTRIP = GL_TRIANGLE_STRIP,
		LINELIST = GL_LINES,
		LINESTRIP = GL_LINE_STRIP,
		POINTLIST = GL_POINTS
	};

	enum class DataType
	{
		INT = GL_INT,
		UINT = GL_UNSIGNED_INT,
		BYTE = GL_BYTE,
		UBYTE = GL_UNSIGNED_BYTE,
		SHORT = GL_SHORT,
		USHORT = GL_UNSIGNED_SHORT,
		FLOAT = GL_FLOAT
	};

	enum class FormatType
	{
		D24_S8 = GL_DEPTH24_STENCIL8,
		RGBA32F = GL_RGBA32F,
		RGB32F = GL_RGB32F,
		RG32F = GL_RG32F,
		R32F = GL_R32F,
		R32U = GL_R32UI,
		RGBA = GL_RGBA8,
		RGB = GL_RGB8,
		RG = GL_RG8,
		R = GL_R8,
		BGRA = GL_BGRA
	};

	enum class WrapMode
	{
		CLAMP = GL_CLAMP_TO_EDGE,
		CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
		CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
		REPEAT = GL_REPEAT
	};

	enum class FilterMode
	{
		NEAREST = GL_NEAREST,
		LINEAR = GL_LINEAR,
		NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
		LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
	};

	enum class ClearBit
	{
		COLOR = GL_COLOR_BUFFER_BIT,
		DEPTH = GL_DEPTH_BUFFER_BIT,
		STENCIL = GL_STENCIL_BUFFER_BIT,
		COLOR_DEPTH = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
		DEPTH_STENCIL = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
		COLOR_DEPTH_STENCIL = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
	};

	enum class DepthFuncs
	{
		ALWAYS = GL_ALWAYS,
		NEVER = GL_NEVER,
		LESS = GL_LESS,
		EQUAL = GL_EQUAL,
		LESS_EQUAL = GL_LEQUAL,
		GREATER = GL_GREATER,
		NOT_EQUAL = GL_NOTEQUAL,
		GREATER_EQUAL = GL_GEQUAL
	};

	enum class StencilOp
	{
		KEEP = GL_KEEP,
		ZERO = GL_ZERO,
		REPLACE = GL_REPLACE,
		INCR = GL_INCR,
		INCR_WRAP = GL_INCR_WRAP,
		DECR = GL_DECR,
		DECR_WRAP = GL_DECR_WRAP,
		INVERT = GL_INVERT
	};

	enum class CullMode
	{
		FRONT = GL_FRONT,
		BACK = GL_BACK,
		NONE = GL_NONE
	};

	enum class Face
	{
		FRONT = GL_FRONT,
		BACK = GL_BACK,
		FRONT_BACK = GL_FRONT_AND_BACK,
		NONE = 0
	};

	enum class WindOrder
	{
		CW = GL_CW,
		CCW = GL_CCW
	};

	enum class InputClass
	{
		PER_VERTEX = 0,
		PER_INSTANCE = 1
	};

	enum class TextureSlot
	{
		TEXTURE0 = GL_TEXTURE0,
		TEXTURE1 = GL_TEXTURE1,
		TEXTURE2 = GL_TEXTURE2,
		TEXTURE3 = GL_TEXTURE3,
		TEXTURE4 = GL_TEXTURE4,
		TEXTURE5 = GL_TEXTURE5,
		TEXTURE6 = GL_TEXTURE6,
		TEXTURE7 = GL_TEXTURE7,
		TEXTURE8 = GL_TEXTURE8,
		TEXTURE9 = GL_TEXTURE9,
		TEXTURE10 = GL_TEXTURE10,
		TEXTURE11 = GL_TEXTURE11,
		TEXTURE12 = GL_TEXTURE12,
		TEXTURE13 = GL_TEXTURE13,
		TEXTURE14 = GL_TEXTURE14,
		TEXTURE15 = GL_TEXTURE15,
		TEXTURE16 = GL_TEXTURE16,
		TEXTURE17 = GL_TEXTURE17,
		TEXTURE18 = GL_TEXTURE18,
		TEXTURE19 = GL_TEXTURE19,
		TEXTURE20 = GL_TEXTURE20,
		TEXTURE21 = GL_TEXTURE21,
		TEXTURE22 = GL_TEXTURE22,
		TEXTURE23 = GL_TEXTURE23,
		TEXTURE24 = GL_TEXTURE24,
		TEXTURE25 = GL_TEXTURE25,
		TEXTURE26 = GL_TEXTURE26,
		TEXTURE27 = GL_TEXTURE27,
		TEXTURE28 = GL_TEXTURE28,
		TEXTURE29 = GL_TEXTURE29,
		TEXTURE30 = GL_TEXTURE30,
		TEXTURE31 = GL_TEXTURE31
	};
}