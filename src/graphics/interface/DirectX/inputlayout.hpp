#pragma once
#include <memory>

#include <rsl/primitives>
#include <rsl/logging>

#include "core/utils/profiler.hpp"

#include "core/assets/assethandle.hpp"
#include "graphics/cache/windowprovider.hpp"
#include "graphics/data/bufferhandle.hpp"
#include "graphics/data/shaderhandle.hpp"
#include "graphics/data/vertexattribute.hpp"
#include "graphics/interface/definitions/window.hpp"
#include "graphics/interface/config.hpp"
#include EnumTypes_HPP_PATH

namespace rythe::rendering
{
	template<typename APIType>
	struct Ishader;

	namespace internal
	{
		struct shader;
	}
}

namespace rythe::rendering::internal
{
	namespace ast = rythe::core::assets;
	namespace log = rsl::log;
	struct inputlayout
	{
		friend struct rendering::Ishader<shader>;
	public:
		unsigned int id = 0;
	private:
		std::vector<D3D11_INPUT_ELEMENT_DESC> elementDesc;
		std::unordered_map<std::string, vertexattribute> m_vertexAttribs;
		DXInputLayout m_layout = nullptr;
		DXBlob m_vsBlob = nullptr;
		window_handle m_windowHandle{nullptr};
	public:
		inputlayout& operator=(const inputlayout& other)
		{
			id = other.id;
			elementDesc = other.elementDesc;
			m_vertexAttribs = other.m_vertexAttribs;
			m_layout = other.m_layout;
			m_vsBlob = other.m_vsBlob;
			m_windowHandle = other.m_windowHandle;
			return *this;
		}
		void initialize(unsigned int numBuffers, shader_handle shader)
		{
			ZoneScopedN("[DX11 Inputlayout] initialize()");
			m_windowHandle = WindowProvider::activeWindow;
			m_vsBlob = shader->getImpl().VS;
		}

		void bind()
		{
			ZoneScopedN("[DX11 Inputlayout] bind()");
			m_windowHandle->devcon->IASetInputLayout(m_layout.Get());
		}

		void unbind()
		{
			ZoneScopedN("[DX11 Inputlayout] unbind()");
			m_windowHandle->devcon->IASetInputLayout(nullptr);
		}

		void setAttributePtr(buffer_handle buf, const std::string& attribName, unsigned int index, FormatType components, unsigned int inputSlot, unsigned int stride, unsigned int offset, InputClass inputClass, unsigned int instancedStep)
		{
			ZoneScopedN("[DX11 Inputlayout] setAttributePtr()");
			m_vertexAttribs.emplace(buf->getName(), vertexattribute{attribName.c_str(), index, components, inputSlot, stride, offset, inputClass, instancedStep});
		}

		void submitAttributes()
		{
			ZoneScopedN("[DX11 Inputlayout] submitAttributes()");
			if (m_vertexAttribs.size() > 0)
			{
				for (auto& [name,attrib] : m_vertexAttribs)
				{
					ZoneScopedN("[DX11 Inputlayout][submitAttributes()] iterating over vertex attributes");
					elementDesc.emplace_back(D3D11_INPUT_ELEMENT_DESC{ attrib.name.c_str(), attrib.index, static_cast<DXGI_FORMAT>(attrib.format), attrib.inputSlot, D3D11_APPEND_ALIGNED_ELEMENT, static_cast<D3D11_INPUT_CLASSIFICATION>(attrib.inputClass),attrib.step });
				}
				CHECKERROR(m_windowHandle->dev->CreateInputLayout(elementDesc.data(), elementDesc.size(), m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), &m_layout), "Failed creating input layout", m_windowHandle->checkError());

				clearAttributes();
			}
			bind();
		}

		void clearAttributes()
		{
			ZoneScopedN("[DX11 Inputlayout] clearAttributes()");
			m_vertexAttribs.clear();
			elementDesc.clear();
		}

		void release()
		{
			ZoneScopedN("[DX11 Inputlayout] release()");
			if (!m_layout)
			{
				return;
			}
			m_layout.Reset();
			clearAttributes();
		}
	};
}
