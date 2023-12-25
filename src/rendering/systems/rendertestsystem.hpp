//#pragma once
//#include <chrono>
//#include <ctime>
//#include <memory>
//#include <iostream>
//#include <filesystem>
//
//#include <rsl/logging>
//#include <rsl/math>
//
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//
//#include "core/core.hpp"
//#include "core/events/defaults/exit_event.hpp"
//#include "rendering/interface/definitions/definitions.hpp"
//#include "rendering/cache/shadercache.hpp"
//#include "rendering/cache/texturecache.hpp"
//#include "rendering/cache/buffercache.hpp"
//#include "rendering/cache/meshcache.hpp"
//#include "rendering/components/spriterenderer.hpp"
//#include "rendering/data/renderingtest.hpp"
//#include "rendering/data/vertex.hpp"
//#include "rendering/systems/testing/CSVWriter.hpp"
//
//
//namespace rythe::rendering
//{
//	class TestRenderer : public core::System<core::transform, sprite_renderer>
//	{
//	public:
//		RenderInterface* m_api;
//
//		static std::vector<std::unique_ptr<rendering_test>> m_testScenes;
//		static int currentScene;
//		static int lastScene;
//
//		static bool initializeTest;
//		static bool updateTest;
//		static bool stopTest;
//
//		bool enableTesting = true;
//
//#if RenderingAPI == RenderingAPI_OGL
//		CSVWriter writer = CSVWriter("resources/logs/ogldata.csv");
//#elif RenderingAPI == RenderingAPI_DX11
//		CSVWriter writer = CSVWriter("resources/logs/dx11data.csv");
//#endif
//		int testCount = 0;
//#ifdef _DEBUG
//		float maxTests = 10000.0f;
//#else
//		float maxTests = 10000.0f;
//#endif
//		float timeSum = 0.0f;
//
//		TestRenderer() = default;
//		virtual ~TestRenderer() = default;
//
//		void setup();
//		void update();
//		void shutdown();
//
//		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//		{
//			if (action == GLFW_PRESS)
//			{
//				lastScene = currentScene;
//				switch (key)
//				{
//				case GLFW_KEY_RIGHT:
//					currentScene++;
//					stopTest = true;
//					updateTest = false;
//					initializeTest = true;
//					break;
//				case GLFW_KEY_LEFT:
//					currentScene--;
//					stopTest = true;
//					updateTest = false;
//					initializeTest = true;
//					break;
//				case GLFW_KEY_1:
//					ShaderCache::reloadShaders();
//					break;
//				}
//			}
//
//			if (currentScene > static_cast<int>(m_testScenes.size() - 1))
//			{
//				currentScene = 0;
//			}
//			else if (currentScene < 0)
//			{
//				currentScene = m_testScenes.size() - 1;
//			}
//		}
//
//		void nextScene()
//		{
//			key_callback(nullptr, GLFW_KEY_RIGHT, 0, GLFW_PRESS, 0);
//		}
//
//		void previousScene()
//		{
//			key_callback(nullptr, GLFW_KEY_LEFT, 0, GLFW_PRESS, 0);
//		}
//	};
//}