//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

bool OpenD3D()
{
	D3D_FEATURE_LEVEL levels[] =
	{
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

	return true;
}

//////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	if(OpenD3D())
	{

	}
	gContext.Release();
	gDevice.Release();
	return 0;
}
