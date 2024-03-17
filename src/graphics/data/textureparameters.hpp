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
		rendering::TextureType targetType = rendering::TextureType::TEXTURE2D;
		rendering::WrapMode wrapModeS = rendering::WrapMode::REPEAT;
		rendering::WrapMode wrapModeT = rendering::WrapMode::REPEAT;
		rendering::WrapMode wrapModeR = rendering::WrapMode::REPEAT;
		rendering::FilterMode minFilterMode = rendering::FilterMode::LINEAR;
		rendering::FilterMode magFilterMode = rendering::FilterMode::LINEAR;
		math::vec4 borderColor = math::vec4(0.0f);
		rendering::FormatType format = rendering::FormatType::RGB;
		rendering::UsageType usage = rendering::UsageType::DEFAULT;
		int textures = 1;
		int mipLevels = 1;
		bool generateMipMaps = false;
	};
}

namespace rythe::rendering
{
	using texture_parameters = core::assets::import_settings<rendering::texture>;

	constexpr core::assets::import_settings<rendering::texture> default_texture_params{};
}