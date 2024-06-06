#pragma once
#include <string>
#include <chrono>
#include <functional>
#include <thread>
#include <future>

#include "core/utils/profiler.hpp"

#include "core/math/math.hpp"
#include "core/logging/logging.hpp"
#include "graphics/data/shadersource.hpp"
#include "graphics/data/bufferhandle.hpp"
#include "graphics/cache/buffercache.hpp"
#include "graphics/interface/OpenGL/oglincludes.hpp"
#include "graphics/interface/OpenGL/shadercompiler.hpp"
#include "graphics/interface/config.hpp"
#include EnumTypes_HPP_PATH

namespace rythe::rendering::internal
{
	struct shader
	{
	public:
		unsigned int programId;
		std::string name;
		bool compiled = false;
	private:
		std::unordered_map<std::string, buffer_handle> m_constBuffers;
		std::unordered_map<int, rsl::id_type> m_idOfLocation;
	public:

		shader() = default;
		shader(shader* other)
		{
			programId = other->programId;
			name = other->name;
		}

		void initialize(const std::string& name, const shader_source& source)
		{
			ZoneScopedN("[OpenGL Shader] initialize()");
			this->name = name;
			programId = glCreateProgram();

			unsigned int vs = 0;
			unsigned int gs = 0;
			unsigned int fs = 0;
			
			//This is the code I need to async
			ShaderCompiler::initialize();
			ShaderCompiler::setSource(source);
			vs = ShaderCompiler::compile(ShaderType::VERTEX);
			gs = ShaderCompiler::compile(ShaderType::GEOMETRY);
			fs = ShaderCompiler::compile(ShaderType::FRAGMENT);
			//auto shaderFuture = std::async(std::launch::async, [=,&vs, &gs, &fs]()
			//	{
			//		vs = ShaderCompiler::compile(ShaderType::VERTEX);
			//		gs = ShaderCompiler::compile(ShaderType::GEOMETRY);
			//		fs = ShaderCompiler::compile(ShaderType::FRAGMENT);
			//	});

			//shaderFuture.wait();

			if (vs != 0)
				glAttachShader(programId, vs);
			if (gs != 0)
				glAttachShader(programId, gs);
			if (fs != 0)
				glAttachShader(programId, fs);

			glLinkProgram(programId);
			glValidateProgram(programId);

			GLint isLinked = 0;
			glGetProgramiv(programId, GL_LINK_STATUS, (int*)&isLinked);
			GLint maxLength = 0;
			glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);
			if (isLinked == GL_FALSE || maxLength != 0)
			{
				maxLength = maxLength > 0 ? maxLength : 1;
				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(programId, maxLength, &maxLength, &infoLog[0]);

				log::error("Shader Linkage failed: {}", infoLog.data());

				glDeleteProgram(programId);

				glDeleteShader(vs);
				glDeleteShader(gs);
				glDeleteShader(fs);
				compiled = false;
				return;
			}

			compiled = true;

			glDeleteShader(vs);
			glDeleteShader(gs);
			glDeleteShader(fs);
		}

		void bind()
		{
			ZoneScopedN("[OpenGL Shader] bind()");
			glUseProgram(programId);

			for (auto [name, handle] : m_constBuffers)
			{
				ZoneScopedN("[OpenGL Shader] [bind()] binding the attached buffers");
				handle->bind();

			}
		}

		void unbind()
		{
			ZoneScopedN("[OpenGL Shader] unbind()");

			for (auto [name, handle] : m_constBuffers)
			{
				ZoneScopedN("[OpenGL Shader] [unbind()] unbinding the attached buffers");
				handle->unbind();
			}
			glUseProgram(0);
		}

		template<typename elementType>
		void setUniform(const std::string& bufferName, int location, elementType data[])
		{
			ZoneScopedN("[OpenGL Shader] setUniform()");
			if (m_constBuffers.count(bufferName) != 0)
			{
				m_constBuffers[bufferName]->bufferData<elementType>(data);
				return;
			}
			buffer_handle buffer = BufferCache::getBuffer(bufferName);
			if (buffer == nullptr)
			{
				addBuffer(BufferCache::createConstantBuffer<elementType>(bufferName, location, rendering::UsageType::STATICDRAW));
				m_constBuffers[bufferName]->bufferData<elementType>(data);
				return;
			}
			else
			{
				addBuffer(buffer);
				m_constBuffers[bufferName]->bufferData<elementType>(data);
				return;
			}

			log::error("No data was buffered, because the buffer {} was not added or does not exist", bufferName);
		}

		void addBuffer(buffer_handle handle)
		{
			ZoneScopedN(" [OpenGL Shader] addBuffer()");
			if (static_cast<internal::TargetType>(handle->getTargetType()) != TargetType::CONSTANT_BUFFER)
			{
				log::error("Buffer is not a constant buffer, this is not supported");
				return;
			}

			if (!m_constBuffers.count(handle->getName()))
			{
				m_constBuffers.emplace(handle->getName(), handle);
			}
		}

		void release()
		{
			ZoneScopedN("[OpenGL Shader] release()");
			glDeleteProgram(programId);
			clearBuffers();
		}

		void clearBuffers()
		{
			ZoneScopedN("[OpenGL Shader] clearBuffers()");
			m_constBuffers.clear();
		}

	private:
		std::future<void> compileAsync(ShaderType type)
		{
			return std::async(std::launch::async, [type] {
				ShaderCompiler::compile(type);
				});
		}

		void createProgram()
		{

		}
	};

}