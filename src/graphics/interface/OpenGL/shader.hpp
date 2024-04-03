#pragma once
#include <string>

#include "core/utils/profiler.hpp"

#include "core/math/math.hpp"
#include "core/logging/logging.hpp"
#include "graphics/data/shadersource.hpp"
#include "graphics/data/bufferhandle.hpp"
#include "graphics/data/shaderinfo.hpp"
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
		//std::unordered_map<rsl::id_type, std::unique_ptr<shader_parameter_base>> m_uniforms;
		std::unordered_map<int, rsl::id_type> m_idOfLocation;
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
			ZoneScopedN("[OpenGL Shader] initialize()");
			this->name = name;
			programId = glCreateProgram();

			ShaderCompiler::initialize();
			unsigned int vs = ShaderCompiler::compile(ShaderType::VERTEX, source);
			unsigned int gs = ShaderCompiler::compile(ShaderType::GEOMETRY, source);
			unsigned int fs = ShaderCompiler::compile(ShaderType::FRAGMENT, source);


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

			//process_io();
		}

		void bind()
		{
			ZoneScopedN("[OpenGL Shader] bind()");
			glUseProgram(programId);

			for (auto [name, handle] : m_constBuffers)
			{
				ZoneScopedN("[OpenGL Shader] [bind()] binding the attached buffers");
				handle->bind();
				glBindBufferBase(GL_UNIFORM_BUFFER, handle->m_impl.bindId, handle->getId());
			}
		}

		void unbind()
		{
			ZoneScopedN("[OpenGL Shader] unbind()");
			glUseProgram(0);

			for (auto [name, handle] : m_constBuffers)
			{
				ZoneScopedN("[OpenGL Shader] [unbind()] unbinding the attached buffers");
				handle->unbind();
				glBindBufferBase(GL_UNIFORM_BUFFER, handle->m_impl.bindId, 0);
			}
		}

		template<typename elementType>
		void setUniform(const std::string& bufferName, int location, elementType data[])
		{
			ZoneScopedN("[OpenGL Shader] setUniform()");
			glUseProgram(programId);
			if (m_constBuffers.count(bufferName) != 0)
			{
				m_constBuffers[bufferName]->bufferData<elementType>(data);
				return;
			}
			auto buffer = BufferCache::getBuffer(bufferName);
			if (buffer == nullptr)
			{
				addBuffer(BufferCache::createConstantBuffer<elementType>(bufferName, location, rendering::UsageType::STATICDRAW));
				m_constBuffers[bufferName]->bufferData<elementType>(data);
				return;
			}

			log::error("No data was buffered, because the buffer {} was not added or does not exist", bufferName);
			glUseProgram(0);
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
		//void process_io()
		//{
		//	GLint numActiveUniforms = 0;
		//	glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

		//	GLint maxUniformNameLength = 0;
		//	glGetProgramiv(programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);

		//	GLchar* uniformNameBuffer = new GLchar[maxUniformNameLength];

		//	rsl::uint textureUnit = 1;

		//	for (int uniformId = 0; uniformId < numActiveUniforms; uniformId++)
		//	{
		//		GLint arraySize = 0; // Use this later for uniform arrays.
		//		GLenum type = 0;
		//		GLsizei nameLength = 0;
		//		glGetActiveUniform(programId, uniformId, (GLsizei)maxUniformNameLength, &nameLength, &arraySize, &type, uniformNameBuffer);

		//		std::string_view name(uniformNameBuffer, nameLength + 1);

		//		if (name.find('[') != std::string_view::npos)             // We don't support uniform arrays yet.
		//			continue;

		//		int location = glGetUniformLocation(programId, uniformNameBuffer);
		//		shader_parameter_base* uniform = nullptr;
		//		switch (type)
		//		{
		//		case GL_SAMPLER_2D:
		//			uniform = new rendering::uniform<texture_handle>(name, type, location, static_cast<rendering::TextureSlot>(textureUnit));
		//			textureUnit++;
		//			break;
		//		case GL_UNSIGNED_INT:
		//			uniform = new rendering::uniform<rsl::uint>(name, type, location);
		//			break;
		//		case GL_FLOAT:
		//			uniform = new rendering::uniform<float>(name, type, location);
		//			break;
		//		case GL_FLOAT_VEC2:
		//			uniform = new rendering::uniform<math::float2>(name, type, location);
		//			break;
		//		case GL_FLOAT_VEC3:
		//			uniform = new rendering::uniform<rsl::math::float3>(name, type, location);
		//			break;
		//		case GL_FLOAT_VEC4:
		//			uniform = new rendering::uniform<math::float4>(name, type, location);
		//			break;
		//		case GL_INT:
		//			uniform = new rendering::uniform<int>(name, type, location);
		//			break;
		//		case GL_INT_VEC2:
		//			uniform = new rendering::uniform<math::ivec2>( name, type, location);
		//			break;
		//		case GL_INT_VEC3:
		//			uniform = new rendering::uniform<math::ivec3>( name, type, location);
		//			break;
		//		case GL_INT_VEC4:
		//			uniform = new rendering::uniform<math::ivec4>( name, type, location);
		//			break;
		//		case GL_BOOL:
		//			uniform = new rendering::uniform<bool>(name, type, location);
		//			break;
		//		case GL_BOOL_VEC2:
		//			uniform = new rendering::uniform<math::bvec2>(name, type, location);
		//			break;
		//		case GL_BOOL_VEC3:
		//			uniform = new rendering::uniform<math::bvec3>(name, type, location);
		//			break;
		//		case GL_BOOL_VEC4:
		//			uniform = new rendering::uniform<math::bvec4>( name, type, location);
		//			break;
		//		case GL_FLOAT_MAT2:
		//			uniform = new rendering::uniform<math::float2x2>(name, type, location);
		//			break;
		//		case GL_FLOAT_MAT3:
		//			uniform = new rendering::uniform<math::float3x3>( name, type, location);
		//			break;
		//		case GL_FLOAT_MAT4:
		//			uniform = new rendering::uniform<math::float4x4>(name, type, location);
		//			break;
		//		default:
		//			continue;
		//		}

		//		// Insert uniform into the uniform list.
		//		auto hashid = rsl::nameHash(name);
		//		m_uniforms[hashid] = std::unique_ptr<shader_parameter_base>(uniform);
		//		m_idOfLocation[location] = hashid;
		//	}

		//	delete[] uniformNameBuffer;
		//}
	};

}