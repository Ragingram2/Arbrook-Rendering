#pragma once
#include <string>

#include <rsl/primitives>
#include <rsl/math>

#include "graphics/data/texturehandle.hpp"
#include "graphics/interface/definitions/enumtypes.hpp"

namespace rythe::rendering
{
	//struct shader_handle; 

	//struct shader_parameter_base
	//{
	//protected:
	//	std::string m_name;
	//	unsigned int m_type;
	//	int m_location;

	//	shader_parameter_base(std::nullptr_t t) : m_name(""), m_type(0), m_location(-1) {}
	//	shader_parameter_base(std::string_view name, unsigned int type, int location) :  m_name(name), m_type(type), m_location(location) {}
	//public:
	//	virtual bool is_valid() const { return m_location != -1; }
	//	virtual unsigned int get_type() const { return m_type; }
	//	virtual std::string get_name() const { return m_name; }
	//	virtual int get_location() const { return m_location; }

	//	bool operator==(const shader_parameter_base& other)
	//	{
	//		return m_name == other.m_name && m_type == other.m_type && m_location == other.m_location;
	//	}

	//	bool operator!=(const shader_parameter_base& other)
	//	{
	//		return m_name != other.m_name || m_type != other.m_type || m_location != other.m_location;
	//	}
	//};

	//template <typename T>
	//struct uniform : public shader_parameter_base
	//{
	//public:
	//	uniform( std::string_view name, unsigned int type, int location) : shader_parameter_base( name, type, location) { }
	//	uniform(std::nullptr_t t) : shader_parameter_base(t) {}
	//	template <typename T>
	//	inline void set_value(shader_handle handle, const T& value)
	//	{
	//		if (is_valid())
	//			handle->setUniform<T>(m_name, m_location, &value);
	//	}
	//};

	//template <>
	//struct uniform<texture_handle> : public shader_parameter_base
	//{
	//	TextureSlot m_textureUnit;
	//	texture_handle texture;
	//public:
	//	uniform(std::string_view name, unsigned int type, int location, TextureSlot textureUnit) : shader_parameter_base( name, type, location), m_textureUnit(textureUnit) { }
	//	uniform(std::nullptr_t t) : shader_parameter_base(t) {}

	//	void set_value(shader_handle handle, const texture_handle& value)
	//	{
	//		texture = value;
	//		texture->bind(m_textureUnit);
	//		texture->unbind(m_textureUnit);
	//	}
	//};
}