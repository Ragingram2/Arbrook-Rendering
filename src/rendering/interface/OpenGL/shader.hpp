#pragma once
#include <string>

#include "core/math/math.hpp"
#include "core/logging/logging.hpp"
#include "rendering/data/shadersource.hpp"
#include "rendering/data/bufferhandle.hpp"
#include "rendering/interface/OpenGL/oglincludes.hpp"
#include "rendering/interface/OpenGL/shadercompiler.hpp"
#include "rendering/interface/config.hpp"
#include EnumTypes_HPP_PATH

namespace rythe::rendering::internal
{
	struct shader
	{
	public:
		unsigned int programId;
		std::string name;

	private:
		std::unordered_map<std::string, buffer_handle> m_constBuffers;
	public:
		shader() = default;
		shader(shader* other)
		{
			programId = other->programId;
			name = other->name;
		}
		operator unsigned int() const { return programId; }

		void initialize(const std::string& name, const shader_source& source)
		{
			this->name = name;
			programId = glCreateProgram();

			ShaderCompiler::initialize();
			unsigned int vs = ShaderCompiler::compile(ShaderType::VERTEX, source);
			unsigned int fs = ShaderCompiler::compile(ShaderType::FRAGMENT, source);

			glAttachShader(programId, vs);
			glAttachShader(programId, fs);
			glLinkProgram(programId);
			glValidateProgram(programId);

			GLint isLinked = 0;
			glGetProgramiv(programId, GL_LINK_STATUS, (int*)&isLinked);
			if (isLinked == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(programId, maxLength, &maxLength, &infoLog[0]);

				log::error("Shader Linkage failed: {}",infoLog.data());

				glDeleteProgram(programId);

				glDeleteShader(vs);
				glDeleteShader(fs);

				return;
			}

			glDeleteShader(vs);
			glDeleteShader(fs);

		}

		void bind()
		{
			glUseProgram(programId);

			for (auto [name, handle] : m_constBuffers)
			{
				handle->bind();
				glBindBufferBase(GL_UNIFORM_BUFFER, handle->m_impl.bindId, handle->getId());
			}
		}

		template<typename elementType>
		void setData(const std::string& bufferName, elementType data[])
		{
			if (m_constBuffers.count(bufferName) != 0)
			{
				m_constBuffers[bufferName]->bufferData<elementType>(data);
			}
		}

		void addBuffer(buffer_handle handle)
		{
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
			glDeleteProgram(programId);
			clearBuffers();
		}

		void clearBuffers()
		{
			m_constBuffers.clear();
		}
	};

}