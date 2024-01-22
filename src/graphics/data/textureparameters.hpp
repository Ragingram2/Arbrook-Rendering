#pragma once
#include <string>

#include "core/math/math.hpp"
#include "core/assets/importsettings.hpp"
#include "graphics/interface/definitions/enumtypes.hpp"

namespace rythe::rendering
{
	template<typename APIType>
	struct Itexture;

	namespace internal
	{
		struct texture;
	}

	using texture = Itexture<internal::texture>;
}

namespace rythe::core::assets
{
	template<>
	struct import_settings<rendering::texture>
	{
	public:
		rendering::TargetType targetType = rendering::TargetType::TEXTURE2D;
		rendering::WrapMode wrapModeS = rendering::WrapMode::REPEAT;
		rendering::WrapMode wrapModeT = rendering::WrapMode::REPEAT;
		rendering::WrapMode wrapModeR = rendering::WrapMode::REPEAT;
		rendering::FilterMode minFilterMode = rendering::FilterMode::LINEAR;
		rendering::FilterMode magFilterMode = rendering::FilterMode::LINEAR;
		rendering::FormatType format = rendering::FormatType::RGB;
		rendering::UsageType usage = rendering::UsageType::DEFAULT;
		int mipLevels = 1;
		bool generateMipMaps = false;
	};
}

namespace rythe::rendering
{
	using texture_parameters = core::assets::import_settings<rendering::texture>;

	constexpr core::assets::import_settings<rendering::texture> default_texture_params{
		.targetType = rendering::TargetType::TEXTURE2D,
		.wrapModeS = WrapMode::REPEAT,
		.wrapModeT = WrapMode::REPEAT,
		.wrapModeR = WrapMode::REPEAT,
		.minFilterMode = FilterMode::LINEAR,
		.magFilterMode = FilterMode::LINEAR,
		.format = FormatType::RGBA,
		.usage = UsageType::IMMUTABLE,
		.mipLevels = 1,
		.generateMipMaps = false,
	};
}