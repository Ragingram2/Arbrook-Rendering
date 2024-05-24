#pragma once
#include <memory>
#include <string>
#include <fstream>

#include <rsl/logging>
#include <rsl/math>

#include "core/utils/profiler.hpp"

#include "graphics/cache/texturecache.hpp"
#include "graphics/interface/OpenGL/oglincludes.hpp"
#include "graphics/interface/OpenGL/enumtypes.hpp"
#include "graphics/cache/windowprovider.hpp"
#include "graphics/interface/definitions/framebuffer.hpp"

namespace rythe::rendering::internal
{
	namespace log = rsl::log;
	namespace math = rsl::math;

	struct DepthStencilInfo
	{
		bool DepthTest = false;
		bool DepthWrite = false;
		int DepthWriteMask = 0;
		DepthFuncs DepthFunc = DepthFuncs::LESS;

		struct StencilInfo
		{
			StencilOp StencilOpFail = StencilOp::KEEP;
			StencilOp StencilOpDepthFail = StencilOp::KEEP;
			StencilOp StencilOpPass = StencilOp::KEEP;
			DepthFuncs StencilFunc = DepthFuncs::NEVER;
		};
		bool StencilTest = false;
		int StencilWriteMask = 0;
		int StencilReadMask = 0;
		int ref = 0;
		std::unordered_map<Face, StencilInfo> stencils = { {Face::FRONT,StencilInfo()},{Face::BACK,StencilInfo()} };
	};

	class RenderInterface
	{
	private:
		DepthStencilInfo m_depthStencilInfo;
		window_handle m_windowHandle;
	public:

		void setWindow(window_handle handle)
		{
			ZoneScopedN("[OpenGL RenderInterface] setWindow()");
			m_windowHandle = handle;
			m_windowHandle->makeCurrent();
		}

		void initialize(math::ivec2 res, const std::string& name, GLFWwindow* window = nullptr)
		{
			ZoneScopedN("[OpenGL RenderInterface] initialize()");
			log::info("Initializing OpenGL");
			if (!window && WindowProvider::get(name) == nullptr)
			{
				m_windowHandle = WindowProvider::addWindow(name);
				WindowProvider::setActive(name);
			}
			else if (WindowProvider::get(name) != nullptr)
			{
				m_windowHandle = WindowProvider::get(name);
				return;
			}

			if (!glfwInit())
			{
				log::error("GLFW could not initialize");
				return;
			}
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			m_windowHandle->initialize(res, name, window);
			m_windowHandle->makeCurrent();

			GLenum err = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			if (err != 1)
			{
				log::error("Failed to initialize GLAD: Error Code [{}]", err);
				return;
			}



#ifdef _DEBUG
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			if (GLAD_GL_AMD_debug_output)
				glDebugMessageCallbackAMD(&RenderInterface::debugCallbackAMD, nullptr);
			else if (GLAD_GL_KHR_debug)
				glDebugMessageCallback(&RenderInterface::debugCallback, nullptr);
			else if (GLAD_GL_ARB_debug_output)
				glDebugMessageCallbackARB(&RenderInterface::debugCallbackARB, nullptr);
#endif
		}

		void close()
		{

		}

		GLFWwindow* getGlfwWindow()
		{
			ZoneScopedN("[OpenGL RenderInterface] getGlfwWindow()");
			return m_windowHandle->getGlfwWindow();
		}

		window_handle getWindowHandle()
		{
			ZoneScopedN("[OpenGL RenderInterface] getWindowHandle()");
			return m_windowHandle;
		}

		void makeCurrent()
		{
			ZoneScopedN("[OpenGL RenderInterface] makeCurrent()");
			m_windowHandle->makeCurrent();
		}

		void setSwapInterval(int interval)
		{
			ZoneScopedN("[OpenGL RenderInterface] setSwapInterval()");
			m_windowHandle->setSwapInterval(interval);
		}

		bool shouldWindowClose()
		{
			ZoneScopedN("[OpenGL RenderInterface] shouldWindowClose()");
			return m_windowHandle->shouldClose();
		}

		void setWindowTitle(const std::string& name)
		{
			ZoneScopedN("[OpenGL RenderInterface] setWindowTitle()");
			m_windowHandle->setWindowTitle(name);
		}

		void pollEvents()
		{
			ZoneScopedN("[OpenGL RenderInterface] pollEvents()");
			m_windowHandle->pollEvents();
		}

		void swapBuffers()
		{
			ZoneScopedN("[OpenGL RenderInterface] swapBuffers()");
			glfwSwapBuffers(m_windowHandle->getGlfwWindow());
		}

		void drawArrays(PrimitiveType mode, unsigned int startVertex, unsigned int vertexCount)
		{
			ZoneScopedN("[OpenGL RenderInterface] drawArrays()");
			glDrawArrays(static_cast<GLenum>(mode), startVertex, vertexCount);
		}

		void drawArraysInstanced(PrimitiveType mode, unsigned int vertexCount, unsigned int instanceCount, unsigned int startVertex, unsigned int startInstance)
		{
			ZoneScopedN("[OpenGL RenderInterface] drawArraysInstanced()");
			glDrawArraysInstanced(static_cast<GLenum>(mode), startVertex, vertexCount, instanceCount);
		}

		void drawIndexed(PrimitiveType mode, unsigned int indexCount, unsigned int startIndex, unsigned int baseVertex)
		{
			ZoneScopedN("[OpenGL RenderInterface] drawIndexed()");
			glDrawElementsBaseVertex(static_cast<GLenum>(mode), indexCount, static_cast<GLenum>(DataType::UINT), reinterpret_cast<void*>(sizeof(unsigned int) * startIndex), static_cast<GLint>(baseVertex));
		}

		void drawIndexedInstanced(PrimitiveType mode, unsigned int indexCount, unsigned int instanceCount, unsigned int startIndex, unsigned int baseVertex, unsigned int startInstance)
		{
			ZoneScopedN("[OpenGL RenderInterface] drawIndexedInstanced()");
			glDrawElementsInstanced(static_cast<GLenum>(mode), indexCount, static_cast<GLenum>(DataType::UINT), reinterpret_cast<void*>(sizeof(unsigned int) * startIndex), instanceCount);
		}

		void flush()
		{
			glFlush();
		}

		void clear(bool clearColor, internal::DepthClearBit flags)
		{
			ZoneScopedN("[OpenGL RenderInterface] clear()");
			if (clearColor)
				glClear(GL_COLOR_BUFFER_BIT | static_cast<int>(flags));
			else
				glClear(static_cast<int>(flags));
		}

		void setClearColor(math::vec4 color)
		{
			ZoneScopedN("[OpenGL RenderInterface] setClearColor()");
			glClearColor(color.r, color.g, color.b, color.a);
		}

		void setViewport(float numViewPorts = 1, float leftX = 0, float leftY = 0, float width = 0, float height = 0, float minDepth = 0, float maxDepth = 1)
		{
			ZoneScopedN("[OpenGL RenderInterface] setViewport()");
			if (width <= 0)
				width = m_windowHandle->getResolution().x;

			if (height <= 0)
				height = m_windowHandle->getResolution().y;

			glViewport(leftX, leftY, width, height);
			glDepthRange(minDepth, maxDepth);
		}

		math::vec4 readPixel(rendering::framebuffer& fbo, int x, int y, int width, int height)
		{
			fbo.bind();
			unsigned char res[4];
			GLint viewport[4];

			glGetIntegerv(GL_VIEWPORT, viewport);
			glReadPixels(x, viewport[3] - y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &res);
			fbo.unbind();

			return math::vec4(res[0], res[1], res[2], res[3]);
		}

		void cullFace(CullMode mode = CullMode::NONE)
		{
			ZoneScopedN("[OpenGL RenderInterface] cullFace()");
			switch (mode)
			{
			case CullMode::FRONT:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;
			case CullMode::BACK:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;
			case CullMode::NONE:
				glDisable(GL_CULL_FACE);
			default:
				break;
			}
		}

		void setWindOrder(WindOrder order)
		{
			ZoneScopedN("[OpenGL RenderInterface] setWindOrder()");
			glFrontFace(static_cast<GLenum>(order));
		}

		void depthTest(bool enable)
		{
			ZoneScopedN("[OpenGL RenderInterface] depthTest()");
			m_depthStencilInfo.DepthTest = enable;
		}

		void depthWrite(bool enable)
		{
			ZoneScopedN("[OpenGL RenderInterface] depthWrite()");
			m_depthStencilInfo.DepthWrite = enable;
		}

		void setStencilMask(int mask)
		{
			ZoneScopedN("[OpenGL RenderInterface] setStencilMask()");
			m_depthStencilInfo.StencilWriteMask = mask;
		}

		void setDepthFunction(DepthFuncs function)
		{
			ZoneScopedN("[OpenGL RenderInterface] setDepthFunction()");
			m_depthStencilInfo.DepthFunc = function;
		}

		void stencilTest(bool enable)
		{
			ZoneScopedN("[OpenGL RenderInterface] stencilTest()");
			m_depthStencilInfo.StencilTest = enable;
		}

		void setStencilOp(Face face, StencilOp fail, StencilOp  zfail, StencilOp  zpass)
		{
			ZoneScopedN("[OpenGL RenderInterface] setStencilOp()");
			auto& stencilInfo = m_depthStencilInfo.stencils[face];
			stencilInfo.StencilOpFail = fail;
			stencilInfo.StencilOpDepthFail = zfail;
			stencilInfo.StencilOpPass = zpass;
		}

		void setStencilFunction(Face face, DepthFuncs func, unsigned int ref, unsigned int mask)
		{
			ZoneScopedN("[OpenGL RenderInterface] setStencilFunction()");
			m_depthStencilInfo.StencilReadMask = mask;
			m_depthStencilInfo.ref = ref;
			m_depthStencilInfo.stencils[face].StencilFunc = func;
		}

		void updateDepthStencil()
		{
			ZoneScopedN("[OpenGL RenderInterface] updateDepthStencil()");
			if (m_depthStencilInfo.DepthTest)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);

			glDepthMask(m_depthStencilInfo.DepthWrite);
			glStencilMask(m_depthStencilInfo.StencilWriteMask);
			glDepthFunc(static_cast<GLenum>(m_depthStencilInfo.DepthFunc));

			if (m_depthStencilInfo.StencilTest)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);

			for (auto& [face, stencil] : m_depthStencilInfo.stencils)
			{
				glStencilOpSeparate(static_cast<GLenum>(face), static_cast<GLenum>(stencil.StencilOpFail), static_cast<GLenum>(stencil.StencilOpDepthFail), static_cast<GLenum>(stencil.StencilOpPass));
				glStencilFuncSeparate(static_cast<GLenum>(face), static_cast<GLenum>(stencil.StencilFunc), m_depthStencilInfo.ref, m_depthStencilInfo.StencilReadMask);
			}
		}

		void checkError()
		{
			ZoneScopedN("[OpenGL RenderInterface] checkError()");
			m_windowHandle->checkError();
		}

	private:
		static void debugCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int  severity, int length, const char* message, const void* userparam);
		static void debugCallbackARB(unsigned int source, unsigned int type, unsigned int id, unsigned int  severity, int length, const char* message, const void* userparam);
		static void debugCallbackAMD(unsigned int id, unsigned int category, unsigned int  severity, int length, const char* message, void* userparam);
	};

	inline void RenderInterface::debugCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, const void* userparam)
	{
		if (id == 131185) // Filter out annoying Nvidia message of: Buffer you made will use VRAM because you told us that you want it to allocate VRAM.
			return;

		rsl::cstring s;
		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
			s = "OpenGL";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			s = "Shader compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			s = "Third party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			s = "Application";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			s = "Other";
			break;
		default:
			s = "Unknown";
			break;
		}

		rsl::cstring t;

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			t = "Error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			t = "Deprecation";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			t = "Undefined behavior";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			t = "Performance";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			t = "Portability";
			break;
		case GL_DEBUG_TYPE_MARKER:
			t = "Marker";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			t = "Push";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			t = "Pop";
			break;
		case GL_DEBUG_TYPE_OTHER:
			t = "Misc";
			break;
		default:
			t = "Unknown";
			break;
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			log::error("[{}-{}] {}: {}", s, t, id, message);
			__debugbreak();
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			log::warn("[{}-{}] {}: {}", s, t, id, message);
			break;
		case GL_DEBUG_SEVERITY_LOW:
			log::debug("[{}-{}] {}: {}", s, t, id, message);
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			log::debug("[{}-{}] {}: {}", s, t, id, message);
			break;
		default:
			log::debug("[{}-{}] {}: {}", s, t, id, message);
			break;
		}
	}
	inline void RenderInterface::debugCallbackARB(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, const void* userparam)
	{
		rsl::cstring s;
		switch (source)
		{
		case GL_DEBUG_SOURCE_API_ARB:
			s = "OpenGL";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
			s = "Window system";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
			s = "Shader compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
			s = "Third party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION_ARB:
			s = "Application";
			break;
		case GL_DEBUG_SOURCE_OTHER_ARB:
			s = "Other";
			break;
		default:
			s = "Unknown";
			break;
		}

		rsl::cstring t;

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR_ARB:
			t = "Error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
			t = "Deprecation";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
			t = "Undefined behavior";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE_ARB:
			t = "Performance";
			break;
		case GL_DEBUG_TYPE_PORTABILITY_ARB:
			t = "Portability";
			break;
		case GL_DEBUG_TYPE_OTHER_ARB:
			t = "Misc";
			break;
		default:
			t = "Unknown";
			break;
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH_ARB:
			log::error("[{}-{}] {}: {}", s, t, id, message);
			break;
		case GL_DEBUG_SEVERITY_MEDIUM_ARB:
			log::warn("[{}-{}] {}: {}", s, t, id, message);
			break;
		case GL_DEBUG_SEVERITY_LOW_ARB:
			log::debug("[{}-{}] {}: {}", s, t, id, message);
			break;
		default:
			log::debug("[{}-{}] {}: {}", s, t, id, message);
			break;
		}
	}
	inline void RenderInterface::debugCallbackAMD(unsigned int id, unsigned int category, unsigned int severity, int length, const char* message, void* userparam)
	{
		rsl::cstring c;
		switch (category)
		{
		case GL_DEBUG_CATEGORY_API_ERROR_AMD:
			c = "OpenGL";
			break;
		case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
			c = "Window system";
			break;
		case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
			c = "Deprecation";
			break;
		case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
			c = "Undefined behavior";
			break;
		case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
			c = "Performance";
			break;
		case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
			c = "Shader compiler";
			break;
		case GL_DEBUG_CATEGORY_APPLICATION_AMD:
			c = "Application";
			break;
		case GL_DEBUG_CATEGORY_OTHER_AMD:
			c = "Other";
			break;
		default:
			c = "Unknown";
			break;
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH_AMD:
			log::error("[{}] {}: {}", c, id, message);
			break;
		case GL_DEBUG_SEVERITY_MEDIUM_AMD:
			log::warn("[{}] {}: {}", c, id, message);
			break;
		case GL_DEBUG_SEVERITY_LOW_AMD:
			log::debug("[{}] {}: {}", c, id, message);
			break;
		default:
			log::debug("[{}] {}: {}", c, id, message);
			break;
		}
	}
}