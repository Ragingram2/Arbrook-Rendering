#pragma once
#include <memory>
#include <string>

#include "rendering/interface/config.hpp"
#include EnumTypes_HPP_PATH

namespace rythe::rendering
{
	struct vertexattribute
	{
	public:
		std::string name;
		unsigned int index;
		internal::FormatType format;
		unsigned int inputSlot;
		unsigned int stride;
		unsigned int offset;
		internal::InputClass inputClass;
		unsigned int step;

	public:
		vertexattribute() = default;
		vertexattribute(const std::string& nam, unsigned int idx, internal::FormatType frmt, unsigned int inputSlot, unsigned int strd, unsigned int offst,internal::InputClass inputClass, unsigned int step) : name(nam), index(idx), format(frmt), inputSlot(inputSlot), stride(strd), offset(offst), inputClass(inputClass), step(step) {}
	};
}