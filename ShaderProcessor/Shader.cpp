//////////////////////////////////////////////////////////////////////
// Make it so it can compile multiple shader types in one run
// Sort out the semantic name annotation parser & emit InputLayoutDefinitions
// Make it a proper Compile type in VS2013

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

using ISMap = std::map<int, char const *>;
using USMap = std::map<uint, char const *>;

//////////////////////////////////////////////////////////////////////

template<typename T, typename U> char const *const &GetFrom(T const &map, U x, char const *default = "Unknown")
{
	auto i = map.find(x);
	return (i != map.end()) ? i->second : default;
}

//////////////////////////////////////////////////////////////////////

static ISMap constant_buffer_type_names =
{
	{ D3D_CT_CBUFFER, "ConstantBuffer" },
	{ D3D_CT_TBUFFER, "TextureBuffer" },
	{ D3D_CT_INTERFACE_POINTERS, "InterfacePointers" },
	{ D3D_CT_RESOURCE_BIND_INFO, "ResourceBindingInfo" }
};

// Binding types:

// SamplerStates		// Do not have 'ConstantBuffers'// PSSetSamplers
// Resources			// May have 'ConstantBuffers'	// D3D11_BIND_SHADER_RESOURCE
// ConstantBuffers		// Do have 'ConstantBuffers'	// D3D11_BIND_CONSTANT_BUFFER

//////////////////////////////////////////////////////////////////////

static USMap shader_input_type_names =
{
	{ D3D_SIT_CBUFFER, "ConstantBuffer" },										// has 'Constant Buffer'	// ConstantBuffer
	{ D3D_SIT_TBUFFER, "TextureBuffer" },										// has 'Constant Buffer'	// Resource
	{ D3D_SIT_TEXTURE, "Texture" },												//							// Resource
	{ D3D_SIT_SAMPLER, "SamplerState" },										//							// SamplerState
	{ D3D_SIT_UAV_RWTYPED, "RW_UAVTyped" },										//							// Resource				// D3D11_BIND_UNORDERED_ACCESS
	{ D3D_SIT_STRUCTURED, "StructuredInput" },									// has 'Constant Buffer'	// Resource
	{ D3D_SIT_UAV_RWSTRUCTURED, "RW_UAVStructured" },														// Resource				// D3D11_BIND_UNORDERED_ACCESS
	{ D3D_SIT_BYTEADDRESS, "ByteAddress" },																	// Resource
	{ D3D_SIT_UAV_RWBYTEADDRESS, "RW_ByteAddress" },														// Resource				// D3D11_BIND_UNORDERED_ACCESS
	{ D3D_SIT_UAV_APPEND_STRUCTURED, "AppendStructured" },						// has 'Constant Buffer'	// Resource
	{ D3D_SIT_UAV_CONSUME_STRUCTURED, "ConsumeStructured" },					// has 'Constant Buffer'	// Resource
	{ D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER, "RW_UAVStructuredWithCounter" }	// has 'Constant Buffer'	// Resource				// D3D11_BIND_UNORDERED_ACCESS
};

//////////////////////////////////////////////////////////////////////

static USMap shader_input_dimension_names =
{
	{ D3D_SRV_DIMENSION_UNKNOWN, "" },
	{ D3D_SRV_DIMENSION_BUFFER, "Buffer" },
	{ D3D_SRV_DIMENSION_TEXTURE1D, "1D" },
	{ D3D_SRV_DIMENSION_TEXTURE1DARRAY, "1DArray" },
	{ D3D_SRV_DIMENSION_TEXTURE2D, "2D" },
	{ D3D_SRV_DIMENSION_TEXTURE2DARRAY, "2DArray" },
	{ D3D_SRV_DIMENSION_TEXTURE2DMS, "2DMS" },
	{ D3D_SRV_DIMENSION_TEXTURE2DMSARRAY, "2DMSArray" },
	{ D3D_SRV_DIMENSION_TEXTURE3D, "3D" },
	{ D3D_SRV_DIMENSION_TEXTURECUBE, "Cube" },
	{ D3D_SRV_DIMENSION_TEXTURECUBEARRAY, "CubeArray" },
	{ D3D_SRV_DIMENSION_BUFFEREX, "BufferEx" }
};

//////////////////////////////////////////////////////////////////////

enum StorageType
{
	Float,
	Half,
	Int,
	UInt,
	Short,
	UShort,
	Byte,
	SByte,
	Norm16,
	Norm8,
	UNorm16,
	UNorm8,
	Typeless32,
	Typeless16,
	Typeless8
};

//////////////////////////////////////////////////////////////////////

struct InputType
{
	char const *name;
	int fieldCount;	// if this is 0, infer from input type
	StorageType storageType;
};

//////////////////////////////////////////////////////////////////////

struct DXGI_FormatDescriptor
{
	DXGI_FORMAT format;
	char const *name;
	uint fields;
	uint bitSize;
	StorageType storageType;

	uint GetTotalSizeInBits() const
	{
		return fields * bitSize;
	}
};

//////////////////////////////////////////////////////////////////////

InputType type_suffix[] =
{
	"Float", 0, Float,
	"Half", 0, Half,
	"Int", 0, Int,
	"UInt", 0, UInt,
	"Short", 0, Short,
	"UShort", 0, UShort,
	"Int8", 0, SByte,
	"SByte", 0, SByte,
	"Byte", 0, UNorm8,
	"UInt8", 0, Byte,
	"Norm16", 0, Norm16,
	"Norm8", 0, Norm8,
	"UNorm16", 0, UNorm16,
	"UNorm8", 0, UNorm8,
	"Typeless32", 0, Typeless32,
	"Typeless16", 0, Typeless16,
	"Typeless8", 0, Typeless8
};

//////////////////////////////////////////////////////////////////////

USMap FormatName =
{
	{ DXGI_FORMAT_UNKNOWN, "DXGI_FORMAT_UNKNOWN" },
	{ DXGI_FORMAT_R32G32B32A32_TYPELESS, "DXGI_FORMAT_R32G32B32A32_TYPELESS" },
	{ DXGI_FORMAT_R32G32B32A32_FLOAT, "DXGI_FORMAT_R32G32B32A32_FLOAT" },
	{ DXGI_FORMAT_R32G32B32A32_UINT, "DXGI_FORMAT_R32G32B32A32_UINT" },
	{ DXGI_FORMAT_R32G32B32A32_SINT, "DXGI_FORMAT_R32G32B32A32_SINT" },
	{ DXGI_FORMAT_R32G32B32_TYPELESS, "DXGI_FORMAT_R32G32B32_TYPELESS" },
	{ DXGI_FORMAT_R32G32B32_FLOAT, "DXGI_FORMAT_R32G32B32_FLOAT" },
	{ DXGI_FORMAT_R32G32B32_UINT, "DXGI_FORMAT_R32G32B32_UINT" },
	{ DXGI_FORMAT_R32G32B32_SINT, "DXGI_FORMAT_R32G32B32_SINT" },
	{ DXGI_FORMAT_R16G16B16A16_TYPELESS, "DXGI_FORMAT_R16G16B16A16_TYPELESS" },
	{ DXGI_FORMAT_R16G16B16A16_FLOAT, "DXGI_FORMAT_R16G16B16A16_FLOAT" },
	{ DXGI_FORMAT_R16G16B16A16_UNORM, "DXGI_FORMAT_R16G16B16A16_UNORM" },
	{ DXGI_FORMAT_R16G16B16A16_UINT, "DXGI_FORMAT_R16G16B16A16_UINT" },
	{ DXGI_FORMAT_R16G16B16A16_SNORM, "DXGI_FORMAT_R16G16B16A16_SNORM" },
	{ DXGI_FORMAT_R16G16B16A16_SINT, "DXGI_FORMAT_R16G16B16A16_SINT" },
	{ DXGI_FORMAT_R32G32_TYPELESS, "DXGI_FORMAT_R32G32_TYPELESS" },
	{ DXGI_FORMAT_R32G32_FLOAT, "DXGI_FORMAT_R32G32_FLOAT" },
	{ DXGI_FORMAT_R32G32_UINT, "DXGI_FORMAT_R32G32_UINT" },
	{ DXGI_FORMAT_R32G32_SINT, "DXGI_FORMAT_R32G32_SINT" },
	{ DXGI_FORMAT_R32G8X24_TYPELESS, "DXGI_FORMAT_R32G8X24_TYPELESS" },
	{ DXGI_FORMAT_D32_FLOAT_S8X24_UINT, "DXGI_FORMAT_D32_FLOAT_S8X24_UINT" },
	{ DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS, "DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS" },
	{ DXGI_FORMAT_X32_TYPELESS_G8X24_UINT, "DXGI_FORMAT_X32_TYPELESS_G8X24_UINT" },
	{ DXGI_FORMAT_R10G10B10A2_TYPELESS, "DXGI_FORMAT_R10G10B10A2_TYPELESS" },
	{ DXGI_FORMAT_R10G10B10A2_UNORM, "DXGI_FORMAT_R10G10B10A2_UNORM" },
	{ DXGI_FORMAT_R10G10B10A2_UINT, "DXGI_FORMAT_R10G10B10A2_UINT" },
	{ DXGI_FORMAT_R11G11B10_FLOAT, "DXGI_FORMAT_R11G11B10_FLOAT" },
	{ DXGI_FORMAT_R8G8B8A8_TYPELESS, "DXGI_FORMAT_R8G8B8A8_TYPELESS" },
	{ DXGI_FORMAT_R8G8B8A8_UNORM, "DXGI_FORMAT_R8G8B8A8_UNORM" },
	{ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB" },
	{ DXGI_FORMAT_R8G8B8A8_UINT, "DXGI_FORMAT_R8G8B8A8_UINT" },
	{ DXGI_FORMAT_R8G8B8A8_SNORM, "DXGI_FORMAT_R8G8B8A8_SNORM" },
	{ DXGI_FORMAT_R8G8B8A8_SINT, "DXGI_FORMAT_R8G8B8A8_SINT" },
	{ DXGI_FORMAT_R16G16_TYPELESS, "DXGI_FORMAT_R16G16_TYPELESS" },
	{ DXGI_FORMAT_R16G16_FLOAT, "DXGI_FORMAT_R16G16_FLOAT" },
	{ DXGI_FORMAT_R16G16_UNORM, "DXGI_FORMAT_R16G16_UNORM" },
	{ DXGI_FORMAT_R16G16_UINT, "DXGI_FORMAT_R16G16_UINT" },
	{ DXGI_FORMAT_R16G16_SNORM, "DXGI_FORMAT_R16G16_SNORM" },
	{ DXGI_FORMAT_R16G16_SINT, "DXGI_FORMAT_R16G16_SINT" },
	{ DXGI_FORMAT_R32_TYPELESS, "DXGI_FORMAT_R32_TYPELESS" },
	{ DXGI_FORMAT_D32_FLOAT, "DXGI_FORMAT_D32_FLOAT" },
	{ DXGI_FORMAT_R32_FLOAT, "DXGI_FORMAT_R32_FLOAT" },
	{ DXGI_FORMAT_R32_UINT, "DXGI_FORMAT_R32_UINT" },
	{ DXGI_FORMAT_R32_SINT, "DXGI_FORMAT_R32_SINT" },
	{ DXGI_FORMAT_R24G8_TYPELESS, "DXGI_FORMAT_R24G8_TYPELESS" },
	{ DXGI_FORMAT_D24_UNORM_S8_UINT, "DXGI_FORMAT_D24_UNORM_S8_UINT" },
	{ DXGI_FORMAT_R24_UNORM_X8_TYPELESS, "DXGI_FORMAT_R24_UNORM_X8_TYPELESS" },
	{ DXGI_FORMAT_X24_TYPELESS_G8_UINT, "DXGI_FORMAT_X24_TYPELESS_G8_UINT" },
	{ DXGI_FORMAT_R8G8_TYPELESS, "DXGI_FORMAT_R8G8_TYPELESS" },
	{ DXGI_FORMAT_R8G8_UNORM, "DXGI_FORMAT_R8G8_UNORM" },
	{ DXGI_FORMAT_R8G8_UINT, "DXGI_FORMAT_R8G8_UINT" },
	{ DXGI_FORMAT_R8G8_SNORM, "DXGI_FORMAT_R8G8_SNORM" },
	{ DXGI_FORMAT_R8G8_SINT, "DXGI_FORMAT_R8G8_SINT" },
	{ DXGI_FORMAT_R16_TYPELESS, "DXGI_FORMAT_R16_TYPELESS" },
	{ DXGI_FORMAT_R16_FLOAT, "DXGI_FORMAT_R16_FLOAT" },
	{ DXGI_FORMAT_D16_UNORM, "DXGI_FORMAT_D16_UNORM" },
	{ DXGI_FORMAT_R16_UNORM, "DXGI_FORMAT_R16_UNORM" },
	{ DXGI_FORMAT_R16_UINT, "DXGI_FORMAT_R16_UINT" },
	{ DXGI_FORMAT_R16_SNORM, "DXGI_FORMAT_R16_SNORM" },
	{ DXGI_FORMAT_R16_SINT, "DXGI_FORMAT_R16_SINT" },
	{ DXGI_FORMAT_R8_TYPELESS, "DXGI_FORMAT_R8_TYPELESS" },
	{ DXGI_FORMAT_R8_UNORM, "DXGI_FORMAT_R8_UNORM" },
	{ DXGI_FORMAT_R8_UINT, "DXGI_FORMAT_R8_UINT" },
	{ DXGI_FORMAT_R8_SNORM, "DXGI_FORMAT_R8_SNORM" },
	{ DXGI_FORMAT_R8_SINT, "DXGI_FORMAT_R8_SINT" },
	{ DXGI_FORMAT_A8_UNORM, "DXGI_FORMAT_A8_UNORM" },
	{ DXGI_FORMAT_R1_UNORM, "DXGI_FORMAT_R1_UNORM" },
	{ DXGI_FORMAT_R9G9B9E5_SHAREDEXP, "DXGI_FORMAT_R9G9B9E5_SHAREDEXP" },
	{ DXGI_FORMAT_R8G8_B8G8_UNORM, "DXGI_FORMAT_R8G8_B8G8_UNORM" },
	{ DXGI_FORMAT_G8R8_G8B8_UNORM, "DXGI_FORMAT_G8R8_G8B8_UNORM" },
	{ DXGI_FORMAT_BC1_TYPELESS, "DXGI_FORMAT_BC1_TYPELESS" },
	{ DXGI_FORMAT_BC1_UNORM, "DXGI_FORMAT_BC1_UNORM" },
	{ DXGI_FORMAT_BC1_UNORM_SRGB, "DXGI_FORMAT_BC1_UNORM_SRGB" },
	{ DXGI_FORMAT_BC2_TYPELESS, "DXGI_FORMAT_BC2_TYPELESS" },
	{ DXGI_FORMAT_BC2_UNORM, "DXGI_FORMAT_BC2_UNORM" },
	{ DXGI_FORMAT_BC2_UNORM_SRGB, "DXGI_FORMAT_BC2_UNORM_SRGB" },
	{ DXGI_FORMAT_BC3_TYPELESS, "DXGI_FORMAT_BC3_TYPELESS" },
	{ DXGI_FORMAT_BC3_UNORM, "DXGI_FORMAT_BC3_UNORM" },
	{ DXGI_FORMAT_BC3_UNORM_SRGB, "DXGI_FORMAT_BC3_UNORM_SRGB" },
	{ DXGI_FORMAT_BC4_TYPELESS, "DXGI_FORMAT_BC4_TYPELESS" },
	{ DXGI_FORMAT_BC4_UNORM, "DXGI_FORMAT_BC4_UNORM" },
	{ DXGI_FORMAT_BC4_SNORM, "DXGI_FORMAT_BC4_SNORM" },
	{ DXGI_FORMAT_BC5_TYPELESS, "DXGI_FORMAT_BC5_TYPELESS" },
	{ DXGI_FORMAT_BC5_UNORM, "DXGI_FORMAT_BC5_UNORM" },
	{ DXGI_FORMAT_BC5_SNORM, "DXGI_FORMAT_BC5_SNORM" },
	{ DXGI_FORMAT_B5G6R5_UNORM, "DXGI_FORMAT_B5G6R5_UNORM" },
	{ DXGI_FORMAT_B5G5R5A1_UNORM, "DXGI_FORMAT_B5G5R5A1_UNORM" },
	{ DXGI_FORMAT_B8G8R8A8_UNORM, "DXGI_FORMAT_B8G8R8A8_UNORM" },
	{ DXGI_FORMAT_B8G8R8X8_UNORM, "DXGI_FORMAT_B8G8R8X8_UNORM" },
	{ DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM, "DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM" },
	{ DXGI_FORMAT_B8G8R8A8_TYPELESS, "DXGI_FORMAT_B8G8R8A8_TYPELESS" },
	{ DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, "DXGI_FORMAT_B8G8R8A8_UNORM_SRGB" },
	{ DXGI_FORMAT_B8G8R8X8_TYPELESS, "DXGI_FORMAT_B8G8R8X8_TYPELESS" },
	{ DXGI_FORMAT_B8G8R8X8_UNORM_SRGB, "DXGI_FORMAT_B8G8R8X8_UNORM_SRGB" },
	{ DXGI_FORMAT_BC6H_TYPELESS, "DXGI_FORMAT_BC6H_TYPELESS" },
	{ DXGI_FORMAT_BC6H_UF16, "DXGI_FORMAT_BC6H_UF16" },
	{ DXGI_FORMAT_BC6H_SF16, "DXGI_FORMAT_BC6H_SF16" },
	{ DXGI_FORMAT_BC7_TYPELESS, "DXGI_FORMAT_BC7_TYPELESS" },
	{ DXGI_FORMAT_BC7_UNORM, "DXGI_FORMAT_BC7_UNORM" },
	{ DXGI_FORMAT_BC7_UNORM_SRGB, "DXGI_FORMAT_BC7_UNORM_SRGB" },
	{ DXGI_FORMAT_AYUV, "DXGI_FORMAT_AYUV" },
	{ DXGI_FORMAT_Y410, "DXGI_FORMAT_Y410" },
	{ DXGI_FORMAT_Y416, "DXGI_FORMAT_Y416" },
	{ DXGI_FORMAT_NV12, "DXGI_FORMAT_NV12" },
	{ DXGI_FORMAT_P010, "DXGI_FORMAT_P010" },
	{ DXGI_FORMAT_P016, "DXGI_FORMAT_P016" },
	{ DXGI_FORMAT_420_OPAQUE, "DXGI_FORMAT_420_OPAQUE" },
	{ DXGI_FORMAT_YUY2, "DXGI_FORMAT_YUY2" },
	{ DXGI_FORMAT_Y210, "DXGI_FORMAT_Y210" },
	{ DXGI_FORMAT_Y216, "DXGI_FORMAT_Y216" },
	{ DXGI_FORMAT_NV11, "DXGI_FORMAT_NV11" },
	{ DXGI_FORMAT_AI44, "DXGI_FORMAT_AI44" },
	{ DXGI_FORMAT_IA44, "DXGI_FORMAT_IA44" },
	{ DXGI_FORMAT_P8, "DXGI_FORMAT_P8" },
	{ DXGI_FORMAT_A8P8, "DXGI_FORMAT_A8P8" },
	{ DXGI_FORMAT_B4G4R4A4_UNORM, "DXGI_FORMAT_B4G4R4A4_UNORM" }
};

//////////////////////////////////////////////////////////////////////

uint SizeOfFormatElement(DXGI_FORMAT format)
{
	switch(format)
	{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return 128;

		case DXGI_FORMAT_R32G32B32_TYPELESS:
		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			return 96;

		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return 64;

		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R11G11B10_FLOAT:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_R16G16_TYPELESS:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return 32;

		case DXGI_FORMAT_R8G8_TYPELESS:
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT:
		case DXGI_FORMAT_B5G6R5_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
			return 16;

		case DXGI_FORMAT_R8_TYPELESS:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
		case DXGI_FORMAT_A8_UNORM:
			return 8;

			// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
			return 128;

			// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
		case DXGI_FORMAT_R1_UNORM:
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
			return 64;

			// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
			return 32;

			// These are compressed, but bit-size information is unclear.
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
			return 32;

		case DXGI_FORMAT_UNKNOWN:
		default:
			throw 0;
	}
}

//////////////////////////////////////////////////////////////////////

static DXGI_FORMAT formats[4][4] =
{
	{
		DXGI_FORMAT_R32_TYPELESS,
		DXGI_FORMAT_R32_UINT,
		DXGI_FORMAT_R32_SINT,
		DXGI_FORMAT_R32_FLOAT
	},
	{
		DXGI_FORMAT_R32G32_TYPELESS,
		DXGI_FORMAT_R32G32_UINT,
		DXGI_FORMAT_R32G32_SINT,
		DXGI_FORMAT_R32G32_FLOAT
	},
	{
		DXGI_FORMAT_R32G32B32_TYPELESS,
		DXGI_FORMAT_R32G32B32_UINT,
		DXGI_FORMAT_R32G32B32_SINT,
		DXGI_FORMAT_R32G32B32_FLOAT
	},
	{
		DXGI_FORMAT_R32G32B32A32_TYPELESS,
		DXGI_FORMAT_R32G32B32A32_UINT,
		DXGI_FORMAT_R32G32B32A32_SINT,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	}
};

//////////////////////////////////////////////////////////////////////

DXGI_FormatDescriptor DXGI_Lookup[] =
{
	{ DXGI_FORMAT_R32G32B32A32_FLOAT, "R32G32B32A32_FLOAT", 4, 32, Float },
	{ DXGI_FORMAT_R32G32B32_FLOAT, "R32G32B32_FLOAT", 3, 32, Float },
	{ DXGI_FORMAT_R32G32_FLOAT, "R32G32_FLOAT", 2, 32, Float },
	{ DXGI_FORMAT_R32_FLOAT, "R32_FLOAT", 1, 32, Float },
	{ DXGI_FORMAT_R32G32B32A32_SINT, "R32G32B32A32_SINT", 4, 32, Int },
	{ DXGI_FORMAT_R32G32B32_SINT, "R32G32B32_SINT", 3, 32, Int },
	{ DXGI_FORMAT_R32G32_SINT, "R32G32_SINT", 2, 32, Int },
	{ DXGI_FORMAT_R32_SINT, "R32_SINT", 1, 32, Int },
	{ DXGI_FORMAT_R32G32B32A32_UINT, "R32G32B32A32_UINT", 4, 32, UInt },
	{ DXGI_FORMAT_R32G32B32_UINT, "R32G32B32_UINT", 3, 32, UInt },
	{ DXGI_FORMAT_R32G32_UINT, "R32G32_UINT", 2, 32, UInt },
	{ DXGI_FORMAT_R32_UINT, "R32_UINT", 1, 32, UInt },
	{ DXGI_FORMAT_R32G32B32A32_TYPELESS, "R32G32B32A32_TYPELESS", 4, 32, Typeless32 },
	{ DXGI_FORMAT_R32G32B32_TYPELESS, "R32G32B32_TYPELESS", 3, 32, Typeless32 },
	{ DXGI_FORMAT_R32G32_TYPELESS, "R32G32_TYPELESS", 2, 32, Typeless32 },
	{ DXGI_FORMAT_R32_TYPELESS, "R32_TYPELESS", 1, 32, Typeless32 },
	{ DXGI_FORMAT_R16G16B16A16_FLOAT, "R16G16B16A16_FLOAT", 4, 16, Half },
	{ DXGI_FORMAT_R16G16_FLOAT, "R16G16_FLOAT", 2, 16, Half },
	{ DXGI_FORMAT_R16_FLOAT, "R16_FLOAT", 1, 16, Half },
	{ DXGI_FORMAT_R16G16B16A16_SINT, "R16G16B16A16_SINT", 4, 16, Short },
	{ DXGI_FORMAT_R16G16_SINT, "R16G16_SINT", 2, 16, Short },
	{ DXGI_FORMAT_R16_SINT, "R16_SINT", 1, 16, Short },
	{ DXGI_FORMAT_R16G16B16A16_UINT, "R16G16B16A16_UINT", 4, 16, UShort },
	{ DXGI_FORMAT_R16G16_UINT, "R16G16_UINT", 2, 16, UShort },
	{ DXGI_FORMAT_R16_UINT, "R16_UINT", 1, 16, UShort },
	{ DXGI_FORMAT_R16G16B16A16_SNORM, "R16G16B16A16_SNORM", 4, 16, Norm16 },
	{ DXGI_FORMAT_R16G16_SNORM, "R16G16_SNORM", 2, 16, Norm16 },
	{ DXGI_FORMAT_R16_SNORM, "R16_SNORM", 1, 16, Norm16 },
	{ DXGI_FORMAT_R16G16B16A16_UNORM, "R16G16B16A16_UNORM", 4, 16, UNorm16 },
	{ DXGI_FORMAT_R16G16_UNORM, "R16G16_UNORM", 2, 16, UNorm16 },
	{ DXGI_FORMAT_R16_UNORM, "R16_UNORM", 1, 16, UNorm16 },
	{ DXGI_FORMAT_R16G16B16A16_TYPELESS, "R16G16B16A16_TYPELESS", 4, 16, Typeless16 },
	{ DXGI_FORMAT_R16G16_TYPELESS, "R16G16_TYPELESS", 2, 16, Typeless16 },
	{ DXGI_FORMAT_R16_TYPELESS, "R16_TYPELESS", 1, 16, Typeless16 },
	{ DXGI_FORMAT_R8G8B8A8_SINT, "R8G8B8A8_SINT", 4, 8, SByte },
	{ DXGI_FORMAT_R8G8_SINT, "R8G8_SINT", 2, 8, SByte },
	{ DXGI_FORMAT_R8_SINT, "R8_SINT", 1, 8, SByte },
	{ DXGI_FORMAT_R8G8B8A8_UINT, "R8G8B8A8_UINT", 4, 8, Byte },
	{ DXGI_FORMAT_R8G8_UINT, "R8G8_UINT", 2, 8, Byte },
	{ DXGI_FORMAT_R8_UINT, "R8_UINT", 1, 8, Byte },
	{ DXGI_FORMAT_R8G8B8A8_SNORM, "R8G8B8A8_SNORM", 4, 8, Norm8 },
	{ DXGI_FORMAT_R8G8_SNORM, "R8G8_SNORM", 2, 8, Norm8 },
	{ DXGI_FORMAT_R8_SNORM, "R8_SNORM", 1, 8, Norm8 },
	{ DXGI_FORMAT_R8G8B8A8_UNORM, "R8G8B8A8_UNORM", 4, 8, UNorm8 },
	{ DXGI_FORMAT_R8G8_UNORM, "R8G8_UNORM", 2, 8, UNorm8 },
	{ DXGI_FORMAT_R8_UNORM, "R8_UNORM", 1, 8, UNorm8 },
	{ DXGI_FORMAT_R8G8B8A8_TYPELESS, "R8G8B8A8_TYPELESS", 4, 8, Typeless8 },
	{ DXGI_FORMAT_R8G8_TYPELESS, "R8G8_TYPELESS", 2, 8, Typeless8 },
	{ DXGI_FORMAT_R8_TYPELESS, "R8_TYPELESS", 1, 8, Typeless8 }
};

//////////////////////////////////////////////////////////////////////

DXGI_FormatDescriptor *GetDXGIDescriptor(DXGI_FORMAT format)
{
	for(uint i = 0; i < _countof(DXGI_Lookup); ++i)
	{
		DXGI_FormatDescriptor &d = DXGI_Lookup[i];
		if(d.format == format)
		{
			return &d;
		}
	}
	return null;
}

//////////////////////////////////////////////////////////////////////

uint GetElementCount(DXGI_FORMAT format)
{
	DXGI_FormatDescriptor *d = GetDXGIDescriptor(format);
	if(d != null)
	{
		return d->fields;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////

DXGI_FORMAT GetDXGI_Format(uint fields, StorageType storageType)
{
	for(uint i = 0; i < _countof(DXGI_Lookup); ++i)
	{
		DXGI_FormatDescriptor &d = DXGI_Lookup[i];
		if(d.fields == fields && d.storageType == storageType)
		{
			return d.format;
		}
	}
	return DXGI_FORMAT_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////

char const *GetFormatName(DXGI_FORMAT format)
{
	return GetFrom(FormatName, format, "DXGI_FORMAT_UNKNOWN");
}

//////////////////////////////////////////////////////////////////////

char const *GetNameOfStorageType(StorageType st)
{
	for(int i = 0; i < _countof(type_suffix); ++i)
	{
		if(st == type_suffix[i].storageType)
		{
			return type_suffix[i].name;
		}
	}
	return null;
}

//////////////////////////////////////////////////////////////////////

Binding *Shader::CreateBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	TRACE("Binding %s at %d is a %s called %s\n", GetFrom(shader_input_type_names, desc.Type), desc.BindPoint, GetFrom(shader_input_type_names, desc.Type), desc.Name);
	switch(desc.Type)
	{
		case D3D_SIT_CBUFFER:
			return new ConstantBufferBinding(this, desc);
		case D3D_SIT_TBUFFER:
			break;
		case D3D_SIT_TEXTURE:
			return new ResourceBinding(this, desc);
		case D3D_SIT_SAMPLER:
			return new SamplerBinding(this, desc);
		case D3D_SIT_UAV_RWTYPED:
			break;
		case D3D_SIT_STRUCTURED:
			break;
		case D3D_SIT_UAV_RWSTRUCTURED:
			break;
		case D3D_SIT_BYTEADDRESS:
			break;
		case D3D_SIT_UAV_RWBYTEADDRESS:
			break;
		case D3D_SIT_UAV_APPEND_STRUCTURED:
			break;
		case D3D_SIT_UAV_CONSUME_STRUCTURED:
			break;
		case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			break;
	}
	return new Binding(this, desc);
}

//////////////////////////////////////////////////////////////////////

HRESULT Shader::CreateBindings()
{
	uint numBindingSlots = mShaderDesc.BoundResources;
	uint numConstantBuffers = mShaderDesc.ConstantBuffers;

	// get all the constant buffers
	// then go through the binding slots and maybe marry them up to a constant buffer

	TRACE("ConstantBuffers: %d\nBindingSlots: %d\n", numConstantBuffers, numBindingSlots);

	D3D11_SHADER_BUFFER_DESC *buffers = new D3D11_SHADER_BUFFER_DESC[numConstantBuffers];

	for(uint i = 0; i < numConstantBuffers; ++i)
	{
		D3D11_SHADER_BUFFER_DESC *buffer = &buffers[i];
		ID3D11ShaderReflectionConstantBuffer *b = mReflector->GetConstantBufferByIndex(i);
		b->GetDesc(buffer);
		TRACE("Constant buffer %d (%s) is %s\n", i, buffer->Name, GetFrom(constant_buffer_type_names, buffer->Type));
	}

	mBindings.clear();

	for(uint i = 0; i < numBindingSlots; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC d;
		mReflector->GetResourceBindingDesc(i, &d);
		Binding *r = CreateBinding(d);
		if(r != null)
		{
			mBindings.push_back(r);
		}
	}

	delete[] buffers;

	ShowAllBindings();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////

Shader::Shader(tstring const &filename)
	: mStartIndex(0)
	, mName(StringFromTString(filename))
{
}

//////////////////////////////////////////////////////////////////////

Shader::~Shader()
{
	Destroy();
}

//////////////////////////////////////////////////////////////////////

static string header = "\nnamespace HLSL\n{\n#pragma pack(push, 4)\n\n";
static string footer = "\n#pragma pack(pop)\n\n} // HLSL\n";
static string constructor = "\t\t// Constructor\n\t\t%s_t()";

//////////////////////////////////////////////////////////////////////

void Shader::Output()
{
	printf("%s", header.c_str());

	printf("\tstruct %s_t", Name().c_str());	// TODO: add : PixelShader or : VertexShader or whatever...
	printf("\n\t{\n");
	for(auto i = mDefinitions.begin(); i != mDefinitions.end(); ++i)
	{
		(*i)->MemberOutput(this->Name());
	}
	for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	{
		(*i)->StaticsOutput();
	}
	for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	{
		(*i)->MemberOutput();
		printf("\n");
	}

	OutputInputElements();
	OutputInputStruct();

	printf(constructor.c_str(), Name().c_str());
	char const *sep = ": ";
	for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	{
		printf("\n\t\t\t%s", sep);
		(*i)->ConstructorOutput();
		sep = ",";
	}

	printf("\t\t{\n\t\t}\n");
	printf("\t};\n\n\tDECLSPEC_SELECTANY %s_t %s;\n", Name().c_str(), Name().c_str());

	printf("%s", footer.c_str());
}

void Shader::OutputInputStruct()
{
	printf("struct Input\n{\n");
	for(uint i = 0; i < mInputElements.size(); ++i)
	{
		D3D11_INPUT_ELEMENT_DESC &d = mInputElements[i];
		DXGI_FormatDescriptor *f = GetDXGIDescriptor(d.Format);
		if(f != 0)
		{
			printf("\t%s %s;\n", "Type", "Name");
		}
	}
	printf("};\n");
}

void Shader::OutputInputElements()
{
	printf("\t\tDECLSPEC_SELECTANY InputElements[%d] =\n\t\t{", mInputElements.size());
	uint i = 0;
	char const *sep = "";
	for(uint i = 0; i < mInputElements.size(); ++i)
	{
		D3D11_INPUT_ELEMENT_DESC &d = mInputElements[i];
		char const *formatName = GetFormatName(d.Format);
		printf("%s\n\t\t\t{ \"%s\", %u, %s, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }", sep, d.SemanticName, d.SemanticIndex, formatName);
		sep = ",";
	}
	printf("\n\t\t};\n\n");
}

//////////////////////////////////////////////////////////////////////

template <typename T> static void CallOutput(std::vector<T *> &vec)
{
	for(auto i = vec.begin(); i != vec.end(); ++i)
	{
		(*i)->Output();
	}
}

//////////////////////////////////////////////////////////////////////

HRESULT Shader::CreateDefinitions()
{
	for(uint i = 0; i < mShaderDesc.ConstantBuffers; ++i)
	{
		TypeDefinition *def = new TypeDefinition(mReflector, i);
		mDefinitions.push_back(def);
		mDefinitionIDs[def->mDesc.Name] = i;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

struct InputMember
{
	string name;			// eg Color
	StorageType type;		// eg Byte
	uint count;				// eg 4		=	Byte4 Color;
};

HRESULT Shader::CreateInputLayout()
{
	int n = mShaderDesc.InputParameters;
	mInputElements.resize(n);
	mInputFields.resize(n);
	int vertexSize = 0;
	for(int i = 0; i < n; ++i)
	{
		InputField &f = mInputFields[i];
		D3D11_SIGNATURE_PARAMETER_DESC desc;
		DX(mReflector->GetInputParameterDesc(i, &desc));
		D3D11_INPUT_ELEMENT_DESC &d = mInputElements[i];
		d.SemanticName = desc.SemanticName;
		d.SemanticIndex = desc.SemanticIndex;
		d.InputSlot = 0;
		d.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		d.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		d.InstanceDataStepRate = 0;
		d.Format = DXGI_FORMAT_UNKNOWN;

		int sourceFields = CountBits(desc.Mask);

		// if they ask, try to find the right storage format
		int fieldCount = 0;
		StorageType storageType;
		TRACE("Semantic name: %s\n", d.SemanticName);

		// get everything up to the last _
		char const *u = strrchr(d.SemanticName, '_');
		string type_annotation;
		string semantic_name;
		if(u != null)
		{
			type_annotation = string(d.SemanticName, u - d.SemanticName);
			semantic_name = string(u + 1);

			// check if it's an auto type one
			for(int i = 0; i < _countof(type_suffix); ++i)
			{
				if(_stricmp(type_annotation.c_str(), type_suffix[i].name) == 0)
				{
					int fc = type_suffix[i].fieldCount;
					fieldCount = (fc == 0) ? sourceFields : fc;
					storageType = type_suffix[i].storageType;
					break;
				}
			}
			if(fieldCount == 0)
			{
				// else see if they want to specify the format explicitly
				for(int i = 0; i < _countof(DXGI_Lookup); ++i)
				{
					if(_stricmp(DXGI_Lookup[i].name, type_annotation.c_str()) == 0)
					{
						fieldCount = DXGI_Lookup[i].fields;
						storageType = DXGI_Lookup[i].storageType;
					}
				}
			}

			// got one?
			if(fieldCount != 0)
			{
				// yes, try to match it to an actual format
				d.Format = GetDXGI_Format(fieldCount, storageType);
			}
		}
		else
		{
			semantic_name = d.SemanticName;
		}
		// got a format yet?
		if(d.Format == DXGI_FORMAT_UNKNOWN)
		{
			// no, use a simple mapping
			fieldCount = sourceFields;
			d.Format = formats[fieldCount - 1][desc.ComponentType];
		}

		if(fieldCount != sourceFields)
		{
			// Error, they specified a format which has a different field count from the source input
		}
		vertexSize += SizeOfFormatElement(d.Format) / 8;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

HRESULT Shader::Create(void const *blob, size_t size)
{
	DX(D3DReflect(blob, size, IID_ID3D11ShaderReflection, (void **)&mReflector));
	mReflector->GetDesc(&mShaderDesc);
	DX(CreateDefinitions());
	DX(CreateBindings());
	DX(CreateInputLayout());
	Output();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

HRESULT Shader::Destroy()
{
	mDefinitions.clear();
	mDefinitionIDs.clear();
	mSamplers.clear();
	mTextures.clear();
	mConstantBuffers.clear();
	mTextureBuffers.clear();
	mTextureBufferIDs.clear();
	mConstBufferIDs.clear();
	mSamplerIDs.clear();
	mTextureIDs.clear();
	mName.clear();
	mBindings.clear();
	mReflector.Release();
	return S_OK;
}

