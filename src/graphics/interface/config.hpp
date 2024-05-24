#pragma once
#include <imgui/backends/imgui_impl_glfw.h>


#ifdef _DEBUG
#define CHECKERROR(hr,text,func) if(FAILED(hr))  { log::error(text); func;}
#else
#define CHECKERROR(hr,text,func) hr;
#endif

#define SAFE_RELEASE(p) if(p) { p->Release() (p) = NULL} 

#ifdef RenderingAPI_OGL
#include <imgui/backends/imgui_impl_opengl3.h>
#define Texture_HPP_PATH "graphics/interface/OpenGL/texture.hpp"
#define TextureParams_HPP_PATH "graphics/interface/OpenGL/textureparameters.hpp"
#define RenderInterface_HPP_PATH "graphics/interface/OpenGL/renderinterface.hpp"
#define InputLayout_HPP_PATH "graphics/interface/OpenGL/inputlayout.hpp"
#define Buffer_HPP_PATH "graphics/interface/OpenGL/buffer.hpp"
#define Shader_HPP_PATH "graphics/interface/OpenGL/shader.hpp"
#define Window_HPP_PATH "graphics/interface/OpenGL/window.hpp"
#define EnumTypes_HPP_PATH "graphics/interface/OpenGL/enumtypes.hpp"
#define ShaderCompiler_HPP_PATH "graphics/interface/OpenGL/shadercompiler.hpp"
#define Framebuffer_HPP_PATH "graphics/interface/OpenGL/framebuffer.hpp"
#endif

#ifdef RenderingAPI_DX11
#include <imgui/backends/imgui_impl_dx11.h>
#define Texture_HPP_PATH "graphics/interface/DirectX/texture.hpp"
#define TextureParams_HPP_PATH "graphics/interface/DirectX/textureparameters.hpp"
#define RenderInterface_HPP_PATH "graphics/interface/DirectX/renderinterface.hpp"
#define InputLayout_HPP_PATH "graphics/interface/DirectX/inputlayout.hpp"
#define Buffer_HPP_PATH "graphics/interface/DirectX/buffer.hpp"
#define Shader_HPP_PATH "graphics/interface/DirectX/shader.hpp"
#define Window_HPP_PATH "graphics/interface/DirectX/window.hpp"
#define EnumTypes_HPP_PATH "graphics/interface/DirectX/enumtypes.hpp"
#define ShaderCompiler_HPP_PATH "graphics/interface/DirectX/shadercompiler.hpp"
#define Framebuffer_HPP_PATH "graphics/interface/DirectX/framebuffer.hpp"
#endif




