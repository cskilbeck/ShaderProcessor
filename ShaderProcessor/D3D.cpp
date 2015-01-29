#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

HRESULT __hr;
DXPtr<ID3D11Device> gDevice;
DXPtr<ID3D11DeviceContext> gContext;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

//////////////////////////////////////////////////////////////////////

static std::map<int, string> level_names =
{
	{ D3D_FEATURE_LEVEL_9_1, "D3D_FEATURE_LEVEL_9_1" },
	{ D3D_FEATURE_LEVEL_9_2, "D3D_FEATURE_LEVEL_9_2" },
	{ D3D_FEATURE_LEVEL_9_3, "D3D_FEATURE_LEVEL_9_3" },
	{ D3D_FEATURE_LEVEL_10_0, "D3D_FEATURE_LEVEL_10_0" },
	{ D3D_FEATURE_LEVEL_10_1, "D3D_FEATURE_LEVEL_10_1" },
	{ D3D_FEATURE_LEVEL_11_0, "D3D_FEATURE_LEVEL_11_0" },
	{ D3D_FEATURE_LEVEL_11_1, "D3D_FEATURE_LEVEL_11_1" }
};


//////////////////////////////////////////////////////////////////////

namespace D3D
{
	bool Open()
	{
		TRACE("D3D::Open\n");

		D3D_FEATURE_LEVEL levels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		D3D_FEATURE_LEVEL level_we_got;

		DXB(D3D11CreateDevice(null,
			D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_REFERENCE,
			null,
			D3D11_CREATE_DEVICE_DEBUG,
			levels,
			ARRAYSIZE(levels),
			D3D11_SDK_VERSION,
			&gDevice,
			&level_we_got,
			&gContext));

		auto i = level_names.find(level_we_got);
		if(i != level_names.end())
		{
			TRACE("%s Device created\n", i->second.c_str());
		}
		else
		{
			TRACE("Unknown D3D feature level!?\n");
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////

	void Close()
	{
		TRACE("D3D::Close\n");

		gDevice.Release();
		gContext.Release();
	}

	//////////////////////////////////////////////////////////////////////

	#if defined(_DEBUG)
	void SetDebugName(ID3D11DeviceChild *child, tchar const *name)
	{
		if(child != null && name != null)
		{
			child->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)_tcslen(name), name);
		}
	}
	#endif
}

