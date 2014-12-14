//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

DXPtr<ID3D11DeviceContext> context;

//////////////////////////////////////////////////////////////////////

bool OpenD3D()
{
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_9_1
	};

	D3D_FEATURE_LEVEL level_we_got;

	DXB(D3D11CreateDevice(NULL,
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_REFERENCE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		levels,
		ARRAYSIZE(levels),
		D3D11_SDK_VERSION,
		&gDevice,
		&level_we_got,
		&context));

	return true;
}

//////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	context.Release();
	gDevice.Release();
	return 0;
}
