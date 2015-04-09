//////////////////////////////////////////////////////////////////////
// Enable fullscreen mode & toggle
// Allow creation without a window (headless: CreateDevice())
// Specify HW or reference driver

#include "DXBase.h"

//////////////////////////////////////////////////////////////////////

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

//////////////////////////////////////////////////////////////////////

using namespace DX;

static std::map<uint, char const *> FormatName =
{
	{ DXGI_FORMAT_UNKNOWN, "UNKNOWN" },
	{ DXGI_FORMAT_R32G32B32A32_TYPELESS, "R32G32B32A32_TYPELESS" },
	{ DXGI_FORMAT_R32G32B32A32_FLOAT, "R32G32B32A32_FLOAT" },
	{ DXGI_FORMAT_R32G32B32A32_UINT, "R32G32B32A32_UINT" },
	{ DXGI_FORMAT_R32G32B32A32_SINT, "R32G32B32A32_SINT" },
	{ DXGI_FORMAT_R32G32B32_TYPELESS, "R32G32B32_TYPELESS" },
	{ DXGI_FORMAT_R32G32B32_FLOAT, "R32G32B32_FLOAT" },
	{ DXGI_FORMAT_R32G32B32_UINT, "R32G32B32_UINT" },
	{ DXGI_FORMAT_R32G32B32_SINT, "R32G32B32_SINT" },
	{ DXGI_FORMAT_R16G16B16A16_TYPELESS, "R16G16B16A16_TYPELESS" },
	{ DXGI_FORMAT_R16G16B16A16_FLOAT, "R16G16B16A16_FLOAT" },
	{ DXGI_FORMAT_R16G16B16A16_UNORM, "R16G16B16A16_UNORM" },
	{ DXGI_FORMAT_R16G16B16A16_UINT, "R16G16B16A16_UINT" },
	{ DXGI_FORMAT_R16G16B16A16_SNORM, "R16G16B16A16_SNORM" },
	{ DXGI_FORMAT_R16G16B16A16_SINT, "R16G16B16A16_SINT" },
	{ DXGI_FORMAT_R32G32_TYPELESS, "R32G32_TYPELESS" },
	{ DXGI_FORMAT_R32G32_FLOAT, "R32G32_FLOAT" },
	{ DXGI_FORMAT_R32G32_UINT, "R32G32_UINT" },
	{ DXGI_FORMAT_R32G32_SINT, "R32G32_SINT" },
	{ DXGI_FORMAT_R32G8X24_TYPELESS, "R32G8X24_TYPELESS" },
	{ DXGI_FORMAT_D32_FLOAT_S8X24_UINT, "D32_FLOAT_S8X24_UINT" },
	{ DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS, "R32_FLOAT_X8X24_TYPELESS" },
	{ DXGI_FORMAT_X32_TYPELESS_G8X24_UINT, "X32_TYPELESS_G8X24_UINT" },
	{ DXGI_FORMAT_R10G10B10A2_TYPELESS, "R10G10B10A2_TYPELESS" },
	{ DXGI_FORMAT_R10G10B10A2_UNORM, "R10G10B10A2_UNORM" },
	{ DXGI_FORMAT_R10G10B10A2_UINT, "R10G10B10A2_UINT" },
	{ DXGI_FORMAT_R11G11B10_FLOAT, "R11G11B10_FLOAT" },
	{ DXGI_FORMAT_R8G8B8A8_TYPELESS, "R8G8B8A8_TYPELESS" },
	{ DXGI_FORMAT_R8G8B8A8_UNORM, "R8G8B8A8_UNORM" },
	{ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, "R8G8B8A8_UNORM_SRGB" },
	{ DXGI_FORMAT_R8G8B8A8_UINT, "R8G8B8A8_UINT" },
	{ DXGI_FORMAT_R8G8B8A8_SNORM, "R8G8B8A8_SNORM" },
	{ DXGI_FORMAT_R8G8B8A8_SINT, "R8G8B8A8_SINT" },
	{ DXGI_FORMAT_R16G16_TYPELESS, "R16G16_TYPELESS" },
	{ DXGI_FORMAT_R16G16_FLOAT, "R16G16_FLOAT" },
	{ DXGI_FORMAT_R16G16_UNORM, "R16G16_UNORM" },
	{ DXGI_FORMAT_R16G16_UINT, "R16G16_UINT" },
	{ DXGI_FORMAT_R16G16_SNORM, "R16G16_SNORM" },
	{ DXGI_FORMAT_R16G16_SINT, "R16G16_SINT" },
	{ DXGI_FORMAT_R32_TYPELESS, "R32_TYPELESS" },
	{ DXGI_FORMAT_D32_FLOAT, "D32_FLOAT" },
	{ DXGI_FORMAT_R32_FLOAT, "R32_FLOAT" },
	{ DXGI_FORMAT_R32_UINT, "R32_UINT" },
	{ DXGI_FORMAT_R32_SINT, "R32_SINT" },
	{ DXGI_FORMAT_R24G8_TYPELESS, "R24G8_TYPELESS" },
	{ DXGI_FORMAT_D24_UNORM_S8_UINT, "D24_UNORM_S8_UINT" },
	{ DXGI_FORMAT_R24_UNORM_X8_TYPELESS, "R24_UNORM_X8_TYPELESS" },
	{ DXGI_FORMAT_X24_TYPELESS_G8_UINT, "X24_TYPELESS_G8_UINT" },
	{ DXGI_FORMAT_R8G8_TYPELESS, "R8G8_TYPELESS" },
	{ DXGI_FORMAT_R8G8_UNORM, "R8G8_UNORM" },
	{ DXGI_FORMAT_R8G8_UINT, "R8G8_UINT" },
	{ DXGI_FORMAT_R8G8_SNORM, "R8G8_SNORM" },
	{ DXGI_FORMAT_R8G8_SINT, "R8G8_SINT" },
	{ DXGI_FORMAT_R16_TYPELESS, "R16_TYPELESS" },
	{ DXGI_FORMAT_R16_FLOAT, "R16_FLOAT" },
	{ DXGI_FORMAT_D16_UNORM, "D16_UNORM" },
	{ DXGI_FORMAT_R16_UNORM, "R16_UNORM" },
	{ DXGI_FORMAT_R16_UINT, "R16_UINT" },
	{ DXGI_FORMAT_R16_SNORM, "R16_SNORM" },
	{ DXGI_FORMAT_R16_SINT, "R16_SINT" },
	{ DXGI_FORMAT_R8_TYPELESS, "R8_TYPELESS" },
	{ DXGI_FORMAT_R8_UNORM, "R8_UNORM" },
	{ DXGI_FORMAT_R8_UINT, "R8_UINT" },
	{ DXGI_FORMAT_R8_SNORM, "R8_SNORM" },
	{ DXGI_FORMAT_R8_SINT, "R8_SINT" },
	{ DXGI_FORMAT_A8_UNORM, "A8_UNORM" },
	{ DXGI_FORMAT_R1_UNORM, "R1_UNORM" },
	{ DXGI_FORMAT_R9G9B9E5_SHAREDEXP, "R9G9B9E5_SHAREDEXP" },
	{ DXGI_FORMAT_R8G8_B8G8_UNORM, "R8G8_B8G8_UNORM" },
	{ DXGI_FORMAT_G8R8_G8B8_UNORM, "G8R8_G8B8_UNORM" },
	{ DXGI_FORMAT_BC1_TYPELESS, "BC1_TYPELESS" },
	{ DXGI_FORMAT_BC1_UNORM, "BC1_UNORM" },
	{ DXGI_FORMAT_BC1_UNORM_SRGB, "BC1_UNORM_SRGB" },
	{ DXGI_FORMAT_BC2_TYPELESS, "BC2_TYPELESS" },
	{ DXGI_FORMAT_BC2_UNORM, "BC2_UNORM" },
	{ DXGI_FORMAT_BC2_UNORM_SRGB, "BC2_UNORM_SRGB" },
	{ DXGI_FORMAT_BC3_TYPELESS, "BC3_TYPELESS" },
	{ DXGI_FORMAT_BC3_UNORM, "BC3_UNORM" },
	{ DXGI_FORMAT_BC3_UNORM_SRGB, "BC3_UNORM_SRGB" },
	{ DXGI_FORMAT_BC4_TYPELESS, "BC4_TYPELESS" },
	{ DXGI_FORMAT_BC4_UNORM, "BC4_UNORM" },
	{ DXGI_FORMAT_BC4_SNORM, "BC4_SNORM" },
	{ DXGI_FORMAT_BC5_TYPELESS, "BC5_TYPELESS" },
	{ DXGI_FORMAT_BC5_UNORM, "BC5_UNORM" },
	{ DXGI_FORMAT_BC5_SNORM, "BC5_SNORM" },
	{ DXGI_FORMAT_B5G6R5_UNORM, "B5G6R5_UNORM" },
	{ DXGI_FORMAT_B5G5R5A1_UNORM, "B5G5R5A1_UNORM" },
	{ DXGI_FORMAT_B8G8R8A8_UNORM, "B8G8R8A8_UNORM" },
	{ DXGI_FORMAT_B8G8R8X8_UNORM, "B8G8R8X8_UNORM" },
	{ DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM, "R10G10B10_XR_BIAS_A2_UNORM" },
	{ DXGI_FORMAT_B8G8R8A8_TYPELESS, "B8G8R8A8_TYPELESS" },
	{ DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, "B8G8R8A8_UNORM_SRGB" },
	{ DXGI_FORMAT_B8G8R8X8_TYPELESS, "B8G8R8X8_TYPELESS" },
	{ DXGI_FORMAT_B8G8R8X8_UNORM_SRGB, "B8G8R8X8_UNORM_SRGB" },
	{ DXGI_FORMAT_BC6H_TYPELESS, "BC6H_TYPELESS" },
	{ DXGI_FORMAT_BC6H_UF16, "BC6H_UF16" },
	{ DXGI_FORMAT_BC6H_SF16, "BC6H_SF16" },
	{ DXGI_FORMAT_BC7_TYPELESS, "BC7_TYPELESS" },
	{ DXGI_FORMAT_BC7_UNORM, "BC7_UNORM" },
	{ DXGI_FORMAT_BC7_UNORM_SRGB, "BC7_UNORM_SRGB" },
	{ DXGI_FORMAT_AYUV, "AYUV" },
	{ DXGI_FORMAT_Y410, "Y410" },
	{ DXGI_FORMAT_Y416, "Y416" },
	{ DXGI_FORMAT_NV12, "NV12" },
	{ DXGI_FORMAT_P010, "P010" },
	{ DXGI_FORMAT_P016, "P016" },
	{ DXGI_FORMAT_420_OPAQUE, "420_OPAQUE" },
	{ DXGI_FORMAT_YUY2, "YUY2" },
	{ DXGI_FORMAT_Y210, "Y210" },
	{ DXGI_FORMAT_Y216, "Y216" },
	{ DXGI_FORMAT_NV11, "NV11" },
	{ DXGI_FORMAT_AI44, "AI44" },
	{ DXGI_FORMAT_IA44, "IA44" },
	{ DXGI_FORMAT_P8, "P8" },
	{ DXGI_FORMAT_A8P8, "A8P8" },
	{ DXGI_FORMAT_B4G4R4A4_UNORM, "B4G4R4A4_UNORM" }
};

static std::map<uint, char const *> scanlineOrderNames =
{
	{ DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, "UNSPECIFIED" },
	{ DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE, "PROGRESSIVE" },
	{ DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST, "UPPER_FIELD_FIRST" },
	{ DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST, "LOWER_FIELD_FIRST" }
};

static std::map<uint, char const *> scalingModeNames =
{
	{ DXGI_MODE_SCALING_UNSPECIFIED, "UNSPECIFIED" },
	{ DXGI_MODE_SCALING_CENTERED, "CENTERED" },
	{ DXGI_MODE_SCALING_STRETCHED, "STRETCHED" }
};

template<typename t, typename u> static char const *GetFrom(t &map, u n)
{
	auto f = map.find(n);
	return (f != map.end()) ? f->second : null;
}

//////////////////////////////////////////////////////////////////////

namespace DX
{
	int32 __hr;

	//////////////////////////////////////////////////////////////////////

	std::map<int, string> D3DLevelNames =
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

#if defined(_DEBUG)
	void SetDebugName(ID3D11DeviceChild *child, tchar const *name)
	{
		if(child != null && name != null)
		{
			DXI(child->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)_tcslen(name), name));
		}
	}
#endif

	char const *GetDXGIFormatName(DXGI_FORMAT format)
	{
		return GetFrom(FormatName, format);
	}

	char const *GetDXGIScanLineOrderingName(DXGI_MODE_SCANLINE_ORDER scanlineOrder)
	{
		return GetFrom(scanlineOrderNames, scanlineOrder);
	}

	char const *GetDXGIScalingModeName(DXGI_MODE_SCALING scalingMode)
	{
		return GetFrom(scalingModeNames, scalingMode);
	}

}

