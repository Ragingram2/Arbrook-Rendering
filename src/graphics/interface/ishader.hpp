#pragma once
#include <string>

#include "core/utils/profiler.hpp"

#include "core/assets/assethandle.hpp"
#include "graphics/interface/definitions/enumtypes.hpp"

namespace rythe::rendering
{
	struct buffer_handle;
	class ShaderCache;
	namespace internal
	{
		struct inputlayout;
		struct shader;
	}

	namespace internal
	{
		struct inputlayout;
	}

	template<typename APIType>
	struct Ishader
	{
		friend class ShaderCache;
		friend class ShaderImporter;
		friend struct internal::inputlayout;
	private:
		APIType m_impl;
	public:
		void bind() { m_impl.bind(); }
		void unbind() { m_impl.unbind(); }
		void addBuffer(buffer_handle handle) { m_impl.addBuffer(handle); }
		template<typename elementType>
		void setUniform(const std::string& bufferName, int location, elementType data[]) { m_impl.setUniform(bufferName, location, data); }
		void release() { m_impl.release(); }
		void clearBuffers() { m_impl.clearBuffers(); }

		std::string getName() { return m_impl.name; }
		unsigned int getId() { return m_impl.programId; }
		APIType& getImpl() { return m_impl; }
	};
}

//namespace rythe::core::assets
//{
//	template<>
//	struct asset_handle<rythe::rendering::Ishader<rythe::rendering::internal::shader>>
//	{
//		rsl::id_type m_id;
//		rythe::rendering::Ishader<rythe::rendering::internal::shader>* m_data = nullptr;
//		asset_handle() = default;
//		asset_handle(rsl::id_type id, rythe::rendering::Ishader<rythe::rendering::internal::shader>* data) : m_id(id), m_data(data) {}
//		asset_handle(rsl::id_type id, rythe::rendering::Ishader<rythe::rendering::internal::shader>& data) : m_id(id), m_data(&data) {}
//		asset_handle(const asset_handle<rythe::rendering::Ishader<rythe::rendering::internal::shader>>& handle) : m_id(handle.m_id), m_data(handle.m_data) {}
//
//		rythe::rendering::Ishader<rythe::rendering::internal::shader>* operator->() { return m_data; }
//		operator rythe::rendering::Ishader<rythe::rendering::internal::shader>& () const { return *m_data; }
//		operator rsl::id_type() const { return m_id; }
//
//		bool operator == (asset_handle handle) { return m_data == handle.m_data; }
//		bool operator != (asset_handle handle) { return !operator==(handle); }
//
//		bool operator == (rythe::rendering::Ishader<rythe::rendering::internal::shader>* data) { return m_data == data; }
//		bool operator !=(rythe::rendering::Ishader<rythe::rendering::internal::shader>* data) { return !operator==(data); }
//
//		
//
//	};
//}