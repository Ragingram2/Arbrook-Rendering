#pragma once
#include <string>

#include "core/math/math.hpp"
#include "rendering/interface/definitions/enumtypes.hpp"

namespace rythe::rendering
{
	struct texture_parameters
	{
	public:
		WrapMode wrapModeS = WrapMode::REPEAT;
		WrapMode wrapModeT = WrapMode::REPEAT;
		WrapMode wrapModeR = WrapMode::REPEAT;
		FilterMode filterMode = FilterMode::LINEAR;
		FormatType format = FormatType::RGB;
		UsageType usage = UsageType::DEFAULT;
		int mipLevels = 1;
		bool generateMipMaps = false;
	};

	constexpr texture_parameters default_params{
		.wrapModeS = WrapMode::REPEAT,
		.wrapModeT = WrapMode::REPEAT,
		.wrapModeR = WrapMode::REPEAT,
		.filterMode = FilterMode::LINEAR,
		.format = FormatType::RGBA,
		.usage = UsageType::IMMUTABLE,
		.mipLevels = 1,
		.generateMipMaps = false
	};
}