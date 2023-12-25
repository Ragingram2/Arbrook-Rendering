#pragma once
#include <imgui/backends/imgui_impl_glfw.h>


#ifdef _DEBUG
#define CHECKERROR(hr,text,func) if(FAILED(hr))  { log::error(text); func; __debugbreak();}
#else
#define CHECKERROR(hr,text,func) hr;
#endif

#ifdef RenderingAPI_OGL
#include <imgui/backends/imgui_impl_opengl3.h>
#define Texture_HPP_PATH "rendering/interface/OpenGL/texture.hpp"
#define TextureParams_HPP_PATH "rendering/interface/OpenGL/textureparameters.hpp"
#define RenderInterface_HPP_PATH "rendering/interface/OpenGL/renderinterface.hpp"
#define InputLayout_HPP_PATH "rendering/interface/OpenGL/inputlayout.hpp"
#define Buffer_HPP_PATH "rendering/interface/OpenGL/buffer.hpp"
#define Shader_HPP_PATH "rendering/interface/OpenGL/shader.hpp"
#define Window_HPP_PATH "rendering/interface/OpenGL/window.hpp"
#define EnumTypes_HPP_PATH "rendering/interface/OpenGL/enumtypes.hpp"
#define ShaderCompiler_HPP_PATH "rendering/interface/OpenGL/shadercompiler.hpp"
#endif

#ifdef RenderingAPI_DX11
#include <imgui/backends/imgui_impl_dx11.h>
#define Texture_HPP_PATH "rendering/interface/DirectX/texture.hpp"
#define TextureParams_HPP_PATH "rendering/interface/DirectX/textureparameters.hpp"
#define RenderInterface_HPP_PATH "rendering/interface/DirectX/renderinterface.hpp"
#define InputLayout_HPP_PATH "rendering/interface/DirectX/inputlayout.hpp"
#define Buffer_HPP_PATH "rendering/interface/DirectX/buffer.hpp"
#define Shader_HPP_PATH "rendering/interface/DirectX/shader.hpp"
#define Window_HPP_PATH "rendering/interface/DirectX/window.hpp"
#define EnumTypes_HPP_PATH "rendering/interface/DirectX/enumtypes.hpp"
#define ShaderCompiler_HPP_PATH "rendering/interface/DirectX/shadercompiler.hpp"
#endif




