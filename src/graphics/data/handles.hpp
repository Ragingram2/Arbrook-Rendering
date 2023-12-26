#pragma once
//#include "graphics/data/model.hpp"
//#include "graphics/interface/ishader.hpp"
//#include "graphics/interface/itexture.hpp"
//#include "graphics/interface/ibuffer.hpp"
//#include "graphics/interface/config.hpp"
//#include Shader_HPP_PATH
//#include Texture_HPP_PATH
//#include Buffer_HPP_PATH

#include "graphics/data/bufferhandle.hpp"
#include "graphics/data/meshhandle.hpp"
#include "graphics/data/modelhandle.hpp"
#include "graphics/data/shaderhandle.hpp"
#include "graphics/data/texturehandle.hpp"
#include "graphics/data/materialhandle.hpp"

namespace rythe::rendering
{
	//template<typename dataType>
	//struct data_handle
	//{
	//	dataType* m_data = nullptr;
	//	data_handle() = default;
	//	data_handle(dataType* data) : m_data(data) {}

	//	dataType* operator->() { return m_data; }
	//	operator dataType& () const { return *m_data; }

	//bool operator ==(dataType* data) { return m_data == data; }
	//bool operator !=(dataType* data) { return m_data != data; }
	//};

	//template<>
	//struct data_handle<Ibuffer<internal::buffer>>
	//{
	//	Ibuffer<internal::buffer>* m_data = nullptr;
	//	data_handle() = default;
	//	data_handle(std::nullptr_t null_ptr) : m_data(null_ptr) {}
	//	data_handle(Ibuffer<internal::buffer>* data) : m_data(data) {}
	//	data_handle(Ibuffer<internal::buffer>& data) : m_data(&data) {}

	//	Ibuffer<internal::buffer>* operator->() { return m_data; }
	//	operator Ibuffer<internal::buffer>& () const { return *m_data; }

	//	bool operator ==(std::nullptr_t null_ptr) { return m_data == nullptr; }
	//	bool operator !=(std::nullptr_t null_ptr) { return m_data != nullptr; }
	//};

	//template<>
	//struct data_handle<Ishader<internal::shader>>
	//{
	//	Ishader<internal::shader>* m_data = nullptr;
	//	data_handle() = default;
	//	data_handle(std::nullptr_t null_ptr) : m_data(null_ptr) {}
	//	data_handle(Ishader<internal::shader>* data) : m_data(data) {}
	//	data_handle(Ishader<internal::shader>& data) : m_data(&data) {}

	//	Ishader<internal::shader>* operator->() { return m_data; }
	//	operator Ishader<internal::shader >& () const { return *m_data; }

	//	bool operator ==(std::nullptr_t null_ptr) { return m_data == nullptr; }
	//	bool operator !=(std::nullptr_t null_ptr) { return m_data != nullptr; }
	//};

	//template<>
	//struct data_handle<Itexture<internal::texture>>
	//{
	//	Itexture<internal::texture>* m_data = nullptr;
	//	data_handle() = default;
	//	data_handle(std::nullptr_t null_ptr) : m_data(null_ptr) {}
	//	data_handle(Itexture<internal::texture>* data) : m_data(data) {}
	//	data_handle(Itexture<internal::texture>& data) : m_data(&data) {}

	//	Itexture<internal::texture>* operator->() { return m_data; }
	//	operator Itexture<internal::texture>& () const { return *m_data; }

	//	bool operator ==(std::nullptr_t null_ptr) { return m_data == nullptr; }
	//	bool operator !=(std::nullptr_t null_ptr) { return m_data != nullptr; }
	//};

	//template<>
	//struct data_handle<model>
	//{
	//	model* m_data = nullptr;
	//	data_handle() = default;
	//	data_handle(std::nullptr_t null_ptr) : m_data(null_ptr) {}
	//	data_handle(model* data) : m_data(data) {}
	//	data_handle(model& data) : m_data(&data) {}

	//	model* operator->() { return m_data; }
	//	operator model& () const { return *m_data; }

	//	bool operator ==(std::nullptr_t null_ptr) { return m_data == nullptr; }
	//	bool operator !=(std::nullptr_t null_ptr) { return m_data != nullptr; }
	//};

	//template<>
	//struct buffer_handle
	//{
	//	Ibuffer<internal::buffer>* m_data = nullptr;
	//	buffer_handle() = default;
	//	buffer_handle(std::nullptr_t null_ptr) : m_data(null_ptr) {}
	//	buffer_handle(Ibuffer<internal::buffer>* data) : m_data(data) {}
	//	buffer_handle(Ibuffer<internal::buffer>& data) : m_data(&data) {}

	//	Ibuffer<internal::buffer>* operator->() { return m_data; }
	//	operator Ibuffer<internal::buffer>& () const { return *m_data; }

	//	bool operator ==(std::nullptr_t null_ptr) { return m_data == nullptr; }
	//	bool operator !=(std::nullptr_t null_ptr) { return m_data != nullptr; }
	//};

	//template<>
	//struct shader_handle
	//{
	//	Ishader<internal::shader>* m_data = nullptr;
	//	shader_handle() = default;
	//	shader_handle(std::nullptr_t null_ptr) : m_data(null_ptr) {}
	//	shader_handle(Ishader<internal::shader>* data) : m_data(data) {}
	//	shader_handle(Ishader<internal::shader>& data) : m_data(&data) {}

	//	Ishader<internal::shader>* operator->() { return m_data; }
	//	operator Ishader<internal::shader >& () const { return *m_data; }

	//	bool operator ==(std::nullptr_t null_ptr) { return m_data == nullptr; }
	//	bool operator !=(std::nullptr_t null_ptr) { return m_data != nullptr; }
	//};

	//template<>
	//struct texture_handle
	//{
	//	Itexture<internal::texture>* m_data = nullptr;
	//	texture_handle() = default;
	//	texture_handle(std::nullptr_t null_ptr) : m_data(null_ptr) {}
	//	texture_handle(Itexture<internal::texture>* data) : m_data(data) {}
	//	texture_handle(Itexture<internal::texture>& data) : m_data(&data) {}

	//	Itexture<internal::texture>* operator->() { return m_data; }
	//	operator Itexture<internal::texture>& () const { return *m_data; }

	//	bool operator ==(std::nullptr_t null_ptr) { return m_data == nullptr; }
	//	bool operator !=(std::nullptr_t null_ptr) { return m_data != nullptr; }
	//};

	////template<>
	//struct model_handle
	//{
	//	model* m_data = nullptr;
	//	model_handle() = default;
	//	model_handle(std::nullptr_t null_ptr) : m_data(null_ptr) {}
	//	model_handle(model* data) : m_data(data) {}
	//	model_handle(model& data) : m_data(&data) {}

	//	model* operator->() { return m_data; }
	//	operator model& () const { return *m_data; }

	//	bool operator ==(std::nullptr_t null_ptr) { return m_data == nullptr; }
	//	bool operator !=(std::nullptr_t null_ptr) { return m_data != nullptr; }
	//};

	//typedef struct data_handle<Ibuffer<internal::buffer>> buffer_handle;
	//typedef struct data_handle<Ishader<internal::shader>> shader_handle;
	//typedef struct data_handle<Itexture<internal::texture>> texture_handle;
	//typedef struct data_handle<model> model_handle;
}