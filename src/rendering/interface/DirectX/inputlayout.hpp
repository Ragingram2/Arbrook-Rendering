#pragma once
#include <memory>

#include <rsl/primitives>
#include <rsl/logging>

#include "rendering/cache/windowprovider.hpp"
#include "rendering/data/shaderhandle.hpp"
#include "rendering/data/bufferhandle.hpp"
#include "rendering/data/vertexattribute.hpp"
#include "rendering/interface/definitions/window.hpp"
#include "rendering/interface/config.hpp"
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
	namespace log = rsl::log;
	struct inputlayout
	{
		friend struct rendering::Ishader<shader>;
	public:
		unsigned int id = 0;
	private:
		std::vector<D3D11_INPUT_ELEMENT_DESC> elementDesc;
		std::unordered_map<std::string, vertexattribute> m_vertexAttribs;
		ID3D11InputLayout* m_layout = nullptr;
		ID3DBlob* m_vsBlob = nullptr;
		window_handle m_windowHandle;
	public:
		void initialize(unsigned int numBuffers, shader_handle shader)
		{
			m_windowHandle = WindowProvider::get(0);
			m_vsBlob = shader->getImpl().VS;
		}

		void bind()
		{
			m_windowHandle->devcon->IASetInputLayout(m_layout);
		}

		void setAttributePtr(buffer_handle buf, const std::string& attribName, unsigned int index, FormatType components, unsigned int inputSlot, unsigned int stride, unsigned int offset, InputClass inputClass, unsigned int instancedStep)
		{
			m_vertexAttribs.emplace(buf->getName(), vertexattribute{attribName.c_str(), index, components, inputSlot, stride, offset, inputClass, instancedStep});
		}

		void submitAttributes()
		{
			if (m_vertexAttribs.size() > 0)
			{
				for (auto& [name,attrib] : m_vertexAttribs)
				{
					elementDesc.emplace_back(D3D11_INPUT_ELEMENT_DESC{ attrib.name.c_str(), attrib.index, static_cast<DXGI_FORMAT>(attrib.format), attrib.inputSlot, D3D11_APPEND_ALIGNED_ELEMENT, static_cast<D3D11_INPUT_CLASSIFICATION>(attrib.inputClass),attrib.step });
				}
				CHECKERROR(m_windowHandle->dev->CreateInputLayout(elementDesc.data(), elementDesc.size(), m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), &m_layout), "Failed creating input layout", m_windowHandle->checkError());

				clearAttributes();
			}
			bind();
		}

		void clearAttributes()
		{
			m_vertexAttribs.clear();
			elementDesc.clear();
		}

		void release()
		{
			if (!m_layout)
			{
				return;
			}
			m_layout->Release();
			m_layout = nullptr;
			clearAttributes();
		}
	};
}
