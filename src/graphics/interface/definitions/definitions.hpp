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

#define CLEAR_PRIORITY 0
#define SKYBOX_PRIORITY 10
#define LIGHT_PRIORITY 20
#define OPAQUE_PRIORITY 30
#define POST_FX_PRIORITY 40
#define SUBMIT_PRIORITY 50
#define UI_PRIORITY 60

#define SV_CAMERA 0
#define SV_LIGHTS SV_CAMERA+1
#define SV_MATERIALS SV_LIGHTS+1
