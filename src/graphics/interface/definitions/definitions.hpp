#pragma once
#include "graphics/interface/definitions/renderinterface.hpp"
#include "graphics/interface/definitions/inputlayout.hpp"
#include "graphics/interface/definitions/shader.hpp"
#include "graphics/interface/definitions/buffer.hpp"
#include "graphics/interface/definitions/texture.hpp"
#include "graphics/interface/definitions/window.hpp"
#include "graphics/interface/definitions/framebuffer.hpp"
#include "graphics/interface/config.hpp"
#include EnumTypes_HPP_PATH

#define Screen_Width 1280.f
#define Screen_Height 720.f

#define CLEAR_PRIORITY 0x0000
#define SKYBOX_PRIORITY 0x0001
#define LIGHT_PRIORITY 0x0002
#define OPAQUE_PRIORITY 0x0003
#define POST_FX_PRIORITY 0x0004
#define SUBMIT_PRIORITY 0x0005
#define UI_PRIORITY 0x0006

#define SV_CAMERA 0
#define SV_LIGHTS SV_CAMERA+1
#define SV_MATERIALS SV_LIGHTS+1
