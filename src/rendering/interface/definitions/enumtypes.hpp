#pragma once
#include "rendering/interface/config.hpp"
#include EnumTypes_HPP_PATH

namespace rythe::rendering
{
	enum class ShaderType
	{
		VERTEX = static_cast<int>(internal::ShaderType::VERTEX),
		FRAGMENT = static_cast<int>(internal::ShaderType::FRAGMENT),
		GEOMETRY = static_cast<int>(internal::ShaderType::GEOMETRY),
		HULL = static_cast<int>(internal::ShaderType::HULL),
		COMPUTE = static_cast<int>(internal::ShaderType::COMPUTE)
	};

	enum class TargetType
	{
		VERTEX_BUFFER = static_cast<int>(internal::TargetType::VERTEX_BUFFER),
		INDEX_BUFFER = static_cast<int>(internal::TargetType::INDEX_BUFFER),
		CONSTANT_BUFFER = static_cast<int>(internal::TargetType::CONSTANT_BUFFER),
		TEXTURE2D = static_cast<int>(internal::TargetType::TEXTURE2D)
	};

	enum class UsageType
	{
		DEFAULT = static_cast<int>(internal::UsageType::DEFAULT),
		IMMUTABLE = static_cast<int>(internal::UsageType::IMMUTABLE),
		STATICDRAW = static_cast<int>(internal::UsageType::STATICDRAW),
		STAGING = static_cast<int>(internal::UsageType::STAGING)
	};

	enum class PrimitiveType
	{
		TRIANGLESLIST = static_cast<int>(internal::PrimitiveType::TRIANGLESLIST),
		TRIANGLESTRIP = static_cast<int>(internal::PrimitiveType::TRIANGLESTRIP),
		LINELIST = static_cast<int>(internal::PrimitiveType::LINELIST),
		LINESTRIP = static_cast<int>(internal::PrimitiveType::LINESTRIP),
		POINTLIST = static_cast<int>(internal::PrimitiveType::POINTLIST)
	};

	enum class DataType
	{
		INT = static_cast<int>(internal::DataType::INT),
		UINT = static_cast<int>(internal::DataType::UINT),
		BYTE = static_cast<int>(internal::DataType::BYTE),
		UBYTE = static_cast<int>(internal::DataType::UBYTE),
		SHORT = static_cast<int>(internal::DataType::SHORT),
		USHORT = static_cast<int>(internal::DataType::USHORT),
		FLOAT = static_cast<int>(internal::DataType::FLOAT)
	};

	enum class FormatType
	{
		D24_S8 = static_cast<int>(internal::FormatType::D24_S8),
		DEPTH_STENCIL = static_cast<int>(internal::FormatType::D24_S8),
		RGBA32F = static_cast<int>(internal::FormatType::RGBA32F),
		RGB32F = static_cast<int>(internal::FormatType::RGB32F),
		RG32F = static_cast<int>(internal::FormatType::RG32F),
		R32F = static_cast<int>(internal::FormatType::R32F),
		R32U = static_cast<int>(internal::FormatType::R32U),
		RGBA = static_cast<int>(internal::FormatType::RGBA),
		RGB = static_cast<int>(internal::FormatType::RGB),
		RG = static_cast<int>(internal::FormatType::RG),
		R = static_cast<int>(internal::FormatType::R),
		BGRA = static_cast<int>(internal::FormatType::BGRA)
	};

	//enum class InternalFormatType
	//{

	//};

	enum class WrapMode
	{
		CLAMP = static_cast<int>(internal::WrapMode::CLAMP),
		CLAMP_TO_BORDER = static_cast<int>(internal::WrapMode::CLAMP_TO_BORDER),
		REPEAT = static_cast<int>(internal::WrapMode::REPEAT)
	};

	enum class FilterMode
	{
		NEAREST = static_cast<int>(internal::FilterMode::NEAREST),
		LINEAR = static_cast<int>(internal::FilterMode::LINEAR),
		NEAREST_MIPMAP_NEAREST = static_cast<int>(internal::FilterMode::NEAREST_MIPMAP_NEAREST),
		NEAREST_MIPMAP_LINEAR = static_cast<int>(internal::FilterMode::NEAREST_MIPMAP_LINEAR),
		LINEAR_MIPMAP_NEAREST = static_cast<int>(internal::FilterMode::LINEAR_MIPMAP_NEAREST),
		LINEAR_MIPMAP_LINEAR = static_cast<int>(internal::FilterMode::LINEAR_MIPMAP_LINEAR)
	};

	enum class ClearBit
	{
		COLOR = static_cast<int>(internal::ClearBit::COLOR),
		DEPTH = static_cast<int>(internal::ClearBit::DEPTH),
		STENCIL = static_cast<int>(internal::ClearBit::STENCIL),
		COLOR_DEPTH = static_cast<int>(internal::ClearBit::COLOR_DEPTH),
		DEPTH_STENCIL = static_cast<int>(internal::ClearBit::DEPTH_STENCIL),
		COLOR_DEPTH_STENCIL = static_cast<int>(internal::ClearBit::COLOR_DEPTH_STENCIL)
	};

	enum class DepthFuncs
	{
		ALWAYS = static_cast<int>(internal::DepthFuncs::ALWAYS),
		NEVER = static_cast<int>(internal::DepthFuncs::NEVER),
		LESS = static_cast<int>(internal::DepthFuncs::LESS),
		EQUAL = static_cast<int>(internal::DepthFuncs::EQUAL),
		LESS_EQUAL = static_cast<int>(internal::DepthFuncs::LESS_EQUAL),
		GREATER = static_cast<int>(internal::DepthFuncs::GREATER),
		NOT_EQUAL = static_cast<int>(internal::DepthFuncs::NOT_EQUAL),
		GREATER_EQUAL = static_cast<int>(internal::DepthFuncs::GREATER_EQUAL)
	};

	enum class StencilOp
	{
		KEEP = static_cast<int>(internal::StencilOp::KEEP),
		ZERO = static_cast<int>(internal::StencilOp::ZERO),
		REPLACE = static_cast<int>(internal::StencilOp::REPLACE),
		INCR = static_cast<int>(internal::StencilOp::INCR),
		INCR_WRAP = static_cast<int>(internal::StencilOp::INCR_WRAP),
		DECR = static_cast<int>(internal::StencilOp::DECR),
		DECR_WRAP = static_cast<int>(internal::StencilOp::DECR_WRAP),
		INVERT = static_cast<int>(internal::StencilOp::INVERT)
	};

	enum class CullMode
	{
		FRONT = static_cast<int>(internal::CullMode::FRONT),
		BACK = static_cast<int>(internal::CullMode::BACK),
		NONE = static_cast<int>(internal::CullMode::NONE)
	};

	enum class Face
	{
		FRONT = static_cast<int>(internal::Face::FRONT),
		BACK = static_cast<int>(internal::Face::BACK),
		FRONT_BACK = static_cast<int>(internal::Face::FRONT_BACK),
		NONE = static_cast<int>(internal::Face::NONE)
	};

	enum class WindOrder
	{
		CW = static_cast<int>(internal::WindOrder::CW),
		CCW = static_cast<int>(internal::WindOrder::CCW)
	};

	enum class InputClass
	{
		PER_VERTEX = static_cast<int>(internal::InputClass::PER_VERTEX),
		PER_INSTANCE = static_cast<int>(internal::InputClass::PER_INSTANCE)
	};

	enum class TextureSlot
	{
		TEXTURE0 = static_cast<int>(internal::TextureSlot::TEXTURE0),
		TEXTURE1 = static_cast<int>(internal::TextureSlot::TEXTURE1),
		TEXTURE2 = static_cast<int>(internal::TextureSlot::TEXTURE2),
		TEXTURE3 = static_cast<int>(internal::TextureSlot::TEXTURE3),
		TEXTURE4 = static_cast<int>(internal::TextureSlot::TEXTURE4),
		TEXTURE5 = static_cast<int>(internal::TextureSlot::TEXTURE5),
		TEXTURE6 = static_cast<int>(internal::TextureSlot::TEXTURE6),
		TEXTURE7 = static_cast<int>(internal::TextureSlot::TEXTURE7),
		TEXTURE8 = static_cast<int>(internal::TextureSlot::TEXTURE8),
		TEXTURE9 = static_cast<int>(internal::TextureSlot::TEXTURE9),
		TEXTURE10 = static_cast<int>(internal::TextureSlot::TEXTURE10),
		TEXTURE11 = static_cast<int>(internal::TextureSlot::TEXTURE11),
		TEXTURE12 = static_cast<int>(internal::TextureSlot::TEXTURE12),
		TEXTURE13 = static_cast<int>(internal::TextureSlot::TEXTURE13),
		TEXTURE14 = static_cast<int>(internal::TextureSlot::TEXTURE14),
		TEXTURE15 = static_cast<int>(internal::TextureSlot::TEXTURE15),
		TEXTURE16 = static_cast<int>(internal::TextureSlot::TEXTURE16),
		TEXTURE17 = static_cast<int>(internal::TextureSlot::TEXTURE17),
		TEXTURE18 = static_cast<int>(internal::TextureSlot::TEXTURE18),
		TEXTURE19 = static_cast<int>(internal::TextureSlot::TEXTURE19),
		TEXTURE20 = static_cast<int>(internal::TextureSlot::TEXTURE20),
		TEXTURE21 = static_cast<int>(internal::TextureSlot::TEXTURE21),
		TEXTURE22 = static_cast<int>(internal::TextureSlot::TEXTURE22),
		TEXTURE23 = static_cast<int>(internal::TextureSlot::TEXTURE23),
		TEXTURE24 = static_cast<int>(internal::TextureSlot::TEXTURE24),
		TEXTURE25 = static_cast<int>(internal::TextureSlot::TEXTURE25),
		TEXTURE26 = static_cast<int>(internal::TextureSlot::TEXTURE26),
		TEXTURE27 = static_cast<int>(internal::TextureSlot::TEXTURE27),
		TEXTURE28 = static_cast<int>(internal::TextureSlot::TEXTURE28),
		TEXTURE29 = static_cast<int>(internal::TextureSlot::TEXTURE29),
		TEXTURE30 = static_cast<int>(internal::TextureSlot::TEXTURE30),
		TEXTURE31 = static_cast<int>(internal::TextureSlot::TEXTURE31)
	};
}