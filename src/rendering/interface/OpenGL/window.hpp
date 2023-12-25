#pragma once
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <rsl/logging>
#include <rsl/math>

#include "rendering/interface/OpenGL/oglincludes.hpp"

namespace rythe::rendering::internal
{
	namespace log = rsl::log;
	namespace math = rsl::math;
	struct window
	{
	private:
		GLFWwindow* m_glfwWindow;
		math::ivec2 m_resolution;
		std::string m_windowName;

	public:
		window() = default;
		window(window& hwnd) : m_glfwWindow(hwnd.getGlfwWindow()), m_resolution(hwnd.m_resolution), m_windowName(hwnd.m_windowName) { }
		window(math::ivec2 res, const std::string& name) : m_resolution(res), m_windowName(name) { }

		void initialize(math::ivec2 res, const std::string& name, GLFWwindow* window = nullptr)
		{
			m_resolution = res;
			m_windowName = name;
			if (!window)
				m_glfwWindow = glfwCreateWindow(res.x, res.y, name.c_str(), NULL, NULL);
			else
				m_glfwWindow = window;
		}

		GLFWwindow* getGlfwWindow()
		{
			return m_glfwWindow;
		}

		void swapBuffers()
		{
			glfwSwapBuffers(m_glfwWindow);
		}

		void setSwapInterval(int interval)
		{
			glfwSwapInterval(interval);
		}

		void pollEvents()
		{
			glfwPollEvents();
		}

		bool shouldClose()
		{
			return glfwWindowShouldClose(m_glfwWindow);
		}

		void setWindowTitle(const std::string& name)
		{
			glfwSetWindowTitle(m_glfwWindow, name.data());
		}

		void makeCurrent()
		{
			glfwMakeContextCurrent(m_glfwWindow);
		}

		math::ivec2 getResolution()
		{
			return m_resolution;
		}

		void checkError()
		{
			GLenum errcode;
			while ((errcode = glGetError()) != GL_NO_ERROR)
			{
				std::string error;
				switch (errcode)
				{
				case GL_INVALID_ENUM:
					error = "INVALID_ENUM";
					break;
				case GL_INVALID_VALUE:
					error = "INVALID_VALUE";
					break;
				case GL_INVALID_OPERATION:
					error = "INVALID_OPERATION";
					break;
				case GL_STACK_OVERFLOW:
					error = "STACK_OVERFLOW";
					break;
				case GL_STACK_UNDERFLOW:
					error = "STACK_UNDERFLOW";
					break;
				case GL_OUT_OF_MEMORY:
					error = "OUT_OF_MEMORY";
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					error = "INVALID_FRAMEBUFFER_OPERATION";
					break;
				}
				log::error("{}: {}", errcode, error);
			}
		}
	};

}