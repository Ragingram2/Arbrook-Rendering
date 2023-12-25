#pragma once
#include "rendering/interface/definitions/renderinterface.hpp"
#include "rendering/interface/definitions/inputlayout.hpp"
#include "rendering/interface/definitions/shader.hpp"
#include "rendering/interface/definitions/buffer.hpp"
#include "rendering/interface/definitions/texture.hpp"
#include "rendering/interface/definitions/window.hpp"
#include "rendering/interface/config.hpp"
#include EnumTypes_HPP_PATH

#define Screen_Width 1280.f
#define Screen_Height 720.f

#define CLEAR_PRIORITY 0x0000
#define SKYBOX_PRIORITY 0x0001
#define LIGHT_PRIORITY 0x0002
#define OPAQUE_PRIORITY 0x0003
#define UI_PRIORITY 0x0004

#define SV_CAMERA 0
#define SV_LIGHTS SV_CAMERA+1
#define SV_MATERIALS SV_LIGHTS+1
