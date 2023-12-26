#include <fstream>
#include <iostream>
#include <string>
#include <list>
#include <memory>

#include <D3Dcompiler.h>
#include <rsl/logging>

namespace rythe::rendering::internal
{
	namespace log = rsl::log;
	class Includer : public ID3DInclude
	{
	public:
		Includer(const std::string& shaderDir)
		{
			m_baseDir = std::format("{}include/", shaderDir);
		}
		virtual ~Includer() = default;

		HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes);
		HRESULT __stdcall Close(LPCVOID pData);
	private:
		std::string m_baseDir;
		std::list<std::unique_ptr<char*>> m_data;
	};

	inline HRESULT Includer::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName,
		LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
	{
		std::ifstream file(m_baseDir + pFileName, std::ios::binary);
		if (!file.is_open())
		{
			*ppData = NULL;
			*pBytes = 0;
			return S_OK;
		}

		file.seekg(0, std::ios::end);
		UINT size = static_cast<UINT>(file.tellg());
		file.seekg(0, std::ios::beg);

		if (size > 0)
		{
			char* buf = new char[size];
			file.read(buf, size);
			*ppData = buf;
			*pBytes = size;
		}
		else
		{
			*ppData = NULL;
			*pBytes = 0;
		}
		return S_OK;
	}

	inline HRESULT Includer::Close(LPCVOID pData)
	{
		char* buf = (char*)pData;
		delete[] buf;
		return S_OK;
	}
}
