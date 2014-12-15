#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

HRESULT __hr;
DXPtr<ID3D11Device> gDevice;
DXPtr<ID3D11DeviceContext> gContext;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")