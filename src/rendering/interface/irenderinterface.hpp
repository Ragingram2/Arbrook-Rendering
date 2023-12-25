#pragma once
#include <memory>
#include <string>

#include "rendering/data/windowhandle.hpp"
#include "rendering/interface/definitions/enumtypes.hpp"

namespace rythe::rendering
{
	template<typename APIType>
	class IRenderInterface
	{
	private:
		APIType m_impl;
	public:
		void initialize(math::ivec2 res, const std::string& name, GLFWwindow* window = nullptr) { m_impl.initialize(res, name, window); }
		void close() { m_impl.close(); }
		GLFWwindow* getGlfwWindow() { return m_impl.getGlfwWindow(); }
		window_handle getWindowHandle() { return m_impl.getWindowHandle(); }
		void makeCurrent() { m_impl.makeCurrent(); }
		void setSwapInterval(int interval) { m_impl.setSwapInterval(interval); }
		bool shouldWindowClose() { return m_impl.shouldWindowClose(); }
		void setWindowTitle(const std::string& name) { m_impl.setWindowTitle(name); }
		void pollEvents() { m_impl.pollEvents(); }
		void swapBuffers() { m_impl.swapBuffers(); }

		void drawArrays(PrimitiveType mode, unsigned int startVertex, unsigned int vertexCount) { m_impl.drawArrays(static_cast<internal::PrimitiveType>(mode), startVertex, vertexCount); }
		void drawArraysInstanced(PrimitiveType mode, unsigned int vertexCount, unsigned int instanceCount, unsigned int startVertex, unsigned int startInstance) { m_impl.drawArraysInstanced(static_cast<internal::PrimitiveType>(mode), vertexCount, instanceCount, startVertex, startInstance); }
		void drawIndexed(PrimitiveType mode, unsigned int indexCount, unsigned int startIndex, unsigned int baseVertex) { m_impl.drawIndexed(static_cast<internal::PrimitiveType>(mode), indexCount, startIndex, baseVertex); }
		void drawIndexedInstanced(PrimitiveType mode, unsigned int indexCount, unsigned int instanceCount, unsigned int startIndex, unsigned int baseVertex, unsigned int startInstance) { m_impl.drawIndexedInstanced(static_cast<internal::PrimitiveType>(mode), indexCount, instanceCount, startIndex, baseVertex, startInstance); }

		void clear(ClearBit flags) { m_impl.clear(static_cast<internal::ClearBit>(flags)); }
		void setClearColor(math::vec4 color) { m_impl.setClearColor(color); }
		void setClearColor(math::vec3 color, float alpha = 1.0f) { m_impl.setClearColor(math::vec4(color, alpha)); }
		void setClearColor(float r, float g, float b, float alpha = 1.0f) { m_impl.setClearColor(math::vec4(r, g, b, alpha)); }
		void setViewport(float numViewPorts = 1, float leftX = 0, float leftY = 0, float width = 0, float height = 0, float minDepth = 0, float maxDepth = 1) { m_impl.setViewport(numViewPorts, leftX, leftY, width, height, minDepth, maxDepth); }

		void cullFace(CullMode mode) { m_impl.cullFace(static_cast<internal::CullMode>(mode)); }
		void setWindOrder(WindOrder order) { m_impl.setWindOrder(static_cast<internal::WindOrder>(order)); }

		void depthTest(bool enable) { m_impl.depthTest(enable); }
		void depthWrite(bool enable) { m_impl.depthWrite(enable); }
		void setStencilMask(int mask) { m_impl.setStencilMask(mask); }
		void setDepthFunction(DepthFuncs function) { m_impl.setDepthFunction(static_cast<internal::DepthFuncs>(function)); }
		void stencilTest(bool enable) { m_impl.stencilTest(enable); }
		void setStencilOp(Face face, StencilOp fail, StencilOp zfail, StencilOp zpass) { m_impl.setStencilOp(static_cast<internal::Face>(face), static_cast<internal::StencilOp>(fail), static_cast<internal::StencilOp>(zfail), static_cast<internal::StencilOp>(zpass)); }
		void setStencilFunction(Face face, DepthFuncs func, unsigned int ref, unsigned int mask) { m_impl.setStencilFunction(static_cast<internal::Face>(face), static_cast<internal::DepthFuncs>(func), ref, mask); }
		void updateDepthStencil() { m_impl.updateDepthStencil(); }

		//void setScissorTest();
		//void setBlend(bool blend);
		//void setBlendParam()
		//void createRenderTarget()

		void checkError() { m_impl.checkError(); }

		void BGFXMode(bool enabled) { m_impl.BGFXMode(enabled); }
	};
}