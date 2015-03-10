
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Shlwapi.h"
#include <regex>
#include <set>

#pragma comment(lib, "Shlwapi.lib")

using namespace Printer;

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
	{ D3D_CT_CBUFFER           , "ConstantBuffer" },
	{ D3D_CT_TBUFFER           , "TextureBuffer" },
	{ D3D_CT_INTERFACE_POINTERS, "InterfacePointers" },
	{ D3D_CT_RESOURCE_BIND_INFO, "ResourceBindingInfo" }
};

// Binding types:

// SamplerStates		// Do not have 'ConstantBuffers'// PSSetSamplers
// Resources			// May have 'ConstantBuffers'	// D3D11_BIND_SHADER_RESOURCE
// ConstantBuffers		// Do have 'ConstantBuffers'	// D3D11_BIND_CONSTANT_BUFFER

//////////////////////////////////////////////////////////////////////

static USMap StorageTypeName =
{
	{ Invalid_type	 , "Invalid" },
	{ Float_type     , "Float" },
	{ Half_type      , "Half" },
	{ Int_type       , "Int" },
	{ UInt_type      , "UInt" },
	{ Short_type     , "Short" },
	{ UShort_type    , "UShort" },
	{ Byte_type      , "Byte" },
	{ SByte_type     , "SByte" },
	{ Norm16_type    , "UShort" },
	{ Norm8_type     , "SByte" },
	{ UNorm16_type   , "UInt16" },
	{ UNorm8_type    , "Byte" },
	{ Typeless32_type, "UInt" },
	{ Typeless16_type, "UShort" },
	{ Typeless8_type , "Byte" }
};

//////////////////////////////////////////////////////////////////////

static USMap shader_input_dimension_names =
{
	{ D3D_SRV_DIMENSION_UNKNOWN         , "" },
	{ D3D_SRV_DIMENSION_BUFFER          , "Buffer" },
	{ D3D_SRV_DIMENSION_TEXTURE1D       , "1D" },
	{ D3D_SRV_DIMENSION_TEXTURE1DARRAY  , "1DArray" },
	{ D3D_SRV_DIMENSION_TEXTURE2D       , "2D" },
	{ D3D_SRV_DIMENSION_TEXTURE2DARRAY  , "2DArray" },
	{ D3D_SRV_DIMENSION_TEXTURE2DMS     , "2DMS" },
	{ D3D_SRV_DIMENSION_TEXTURE2DMSARRAY, "2DMSArray" },
	{ D3D_SRV_DIMENSION_TEXTURE3D       , "3D" },
	{ D3D_SRV_DIMENSION_TEXTURECUBE     , "Cube" },
	{ D3D_SRV_DIMENSION_TEXTURECUBEARRAY, "CubeArray" },
	{ D3D_SRV_DIMENSION_BUFFEREX        , "BufferEx" }
};

//////////////////////////////////////////////////////////////////////

InputType type_suffix[] =
{
	"Invalid"	, Invalid_type,
	"Float"     , Float_type,
	"Half"      , Half_type,
	"Int"       , Int_type,
	"UInt"      , UInt_type,
	"Short"     , Short_type,
	"UShort"    , UShort_type,
	"Int8"      , SByte_type,
	"SByte"     , SByte_type,
	"Byte"      , UNorm8_type,
	"UInt8"     , Byte_type,
	"Norm16"    , Norm16_type,
	"Norm8"     , Norm8_type,
	"UNorm16"   , UNorm16_type,
	"UNorm8"    , UNorm8_type,
	"Typeless32", Typeless32_type,
	"Typeless16", Typeless16_type,
	"Typeless8" , Typeless8_type
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
// supported formats for the input assembler

DXGI_FORMAT supported[] =
{
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_UINT,
	DXGI_FORMAT_R32G32B32A32_SINT,

	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32_SINT,

	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R16G16B16A16_SNORM,
	DXGI_FORMAT_R16G16B16A16_SINT,

	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32_SINT,

	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R10G10B10A2_UINT,

	DXGI_FORMAT_R11G11B10_FLOAT,

	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UINT,
	DXGI_FORMAT_R8G8B8A8_SNORM,
	DXGI_FORMAT_R8G8B8A8_SINT,

	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_R16G16_SNORM,
	DXGI_FORMAT_R16G16_SINT,

	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32_SINT,

	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R8G8_UINT,
	DXGI_FORMAT_R8G8_SNORM,
	DXGI_FORMAT_R8G8_SINT,

	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16_SNORM,
	DXGI_FORMAT_R16_SINT,

	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8_UINT,
	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_R8_SINT,

	DXGI_FORMAT_B8G8R8A8_UNORM,
	DXGI_FORMAT_B8G8R8X8_UNORM,

	//Direct3D 11 hardware optionally supports these formats for input assembler vertex buffer resources :

	DXGI_FORMAT_B5G6R5_UNORM,	//Requires DXGI 1.2 or later.DXGI 1.2 types are only supported on systems with Direct3D 11.1 or later.
	DXGI_FORMAT_B5G5R5A1_UNORM,	//Requires DXGI 1.2 or later.DXGI 1.2 types are only supported on systems with Direct3D 11.1 or later.
	DXGI_FORMAT_B4G4R4A4_UNORM	//Requires DXGI 1.2 or later.DXGI 1.2 types are only supported on systems with Direct3D 11.1 or later.
};


DXGI_FormatDescriptor DXGI_Lookup[] =
{
	{ DXGI_FORMAT_R32G32B32A32_FLOAT   , "R32G32B32A32_FLOAT", 4, 32, Float_type },
	{ DXGI_FORMAT_R32G32B32_FLOAT      , "R32G32B32_FLOAT", 3, 32, Float_type },
	{ DXGI_FORMAT_R32G32_FLOAT         , "R32G32_FLOAT", 2, 32, Float_type },
	{ DXGI_FORMAT_R32_FLOAT            , "R32_FLOAT", 1, 32, Float_type },
	{ DXGI_FORMAT_R32G32B32A32_SINT    , "R32G32B32A32_SINT", 4, 32, Int_type },
	{ DXGI_FORMAT_R32G32B32_SINT       , "R32G32B32_SINT", 3, 32, Int_type },
	{ DXGI_FORMAT_R32G32_SINT          , "R32G32_SINT", 2, 32, Int_type },
	{ DXGI_FORMAT_R32_SINT             , "R32_SINT", 1, 32, Int_type },
	{ DXGI_FORMAT_R32G32B32A32_UINT    , "R32G32B32A32_UINT", 4, 32, UInt_type },
	{ DXGI_FORMAT_R32G32B32_UINT       , "R32G32B32_UINT", 3, 32, UInt_type },
	{ DXGI_FORMAT_R32G32_UINT          , "R32G32_UINT", 2, 32, UInt_type },
	{ DXGI_FORMAT_R32_UINT             , "R32_UINT", 1, 32, UInt_type },
	{ DXGI_FORMAT_R32G32B32A32_TYPELESS, "R32G32B32A32_TYPELESS", 4, 32, Typeless32_type },
	{ DXGI_FORMAT_R32G32B32_TYPELESS   , "R32G32B32_TYPELESS", 3, 32, Typeless32_type },
	{ DXGI_FORMAT_R32G32_TYPELESS      , "R32G32_TYPELESS", 2, 32, Typeless32_type },
	{ DXGI_FORMAT_R32_TYPELESS         , "R32_TYPELESS", 1, 32, Typeless32_type },
	{ DXGI_FORMAT_R16G16B16A16_FLOAT   , "R16G16B16A16_FLOAT", 4, 16, Half_type },
	{ DXGI_FORMAT_R16G16_FLOAT         , "R16G16_FLOAT", 2, 16, Half_type },
	{ DXGI_FORMAT_R16_FLOAT            , "R16_FLOAT", 1, 16, Half_type },
	{ DXGI_FORMAT_R16G16B16A16_SINT    , "R16G16B16A16_SINT", 4, 16, Short_type },
	{ DXGI_FORMAT_R16G16_SINT          , "R16G16_SINT", 2, 16, Short_type },
	{ DXGI_FORMAT_R16_SINT             , "R16_SINT", 1, 16, Short_type },
	{ DXGI_FORMAT_R16G16B16A16_UINT    , "R16G16B16A16_UINT", 4, 16, UShort_type },
	{ DXGI_FORMAT_R16G16_UINT          , "R16G16_UINT", 2, 16, UShort_type },
	{ DXGI_FORMAT_R16_UINT             , "R16_UINT", 1, 16, UShort_type },
	{ DXGI_FORMAT_R16G16B16A16_SNORM   , "R16G16B16A16_SNORM", 4, 16, Norm16_type },
	{ DXGI_FORMAT_R16G16_SNORM         , "R16G16_SNORM", 2, 16, Norm16_type },
	{ DXGI_FORMAT_R16_SNORM            , "R16_SNORM", 1, 16, Norm16_type },
	{ DXGI_FORMAT_R16G16B16A16_UNORM   , "R16G16B16A16_UNORM", 4, 16, UNorm16_type },
	{ DXGI_FORMAT_R16G16_UNORM         , "R16G16_UNORM", 2, 16, UNorm16_type },
	{ DXGI_FORMAT_R16_UNORM            , "R16_UNORM", 1, 16, UNorm16_type },
	{ DXGI_FORMAT_R16G16B16A16_TYPELESS, "R16G16B16A16_TYPELESS", 4, 16, Typeless16_type },
	{ DXGI_FORMAT_R16G16_TYPELESS      , "R16G16_TYPELESS", 2, 16, Typeless16_type },
	{ DXGI_FORMAT_R16_TYPELESS         , "R16_TYPELESS", 1, 16, Typeless16_type },
	{ DXGI_FORMAT_R8G8B8A8_SINT        , "R8G8B8A8_SINT", 4, 8, SByte_type },
	{ DXGI_FORMAT_R8G8_SINT            , "R8G8_SINT", 2, 8, SByte_type },
	{ DXGI_FORMAT_R8_SINT              , "R8_SINT", 1, 8, SByte_type },
	{ DXGI_FORMAT_R8G8B8A8_UINT        , "R8G8B8A8_UINT", 4, 8, Byte_type },
	{ DXGI_FORMAT_R8G8_UINT            , "R8G8_UINT", 2, 8, Byte_type },
	{ DXGI_FORMAT_R8_UINT              , "R8_UINT", 1, 8, Byte_type },
	{ DXGI_FORMAT_R8G8B8A8_SNORM       , "R8G8B8A8_SNORM", 4, 8, Norm8_type },
	{ DXGI_FORMAT_R8G8_SNORM           , "R8G8_SNORM", 2, 8, Norm8_type },
	{ DXGI_FORMAT_R8_SNORM             , "R8_SNORM", 1, 8, Norm8_type },
	{ DXGI_FORMAT_R8G8B8A8_UNORM       , "R8G8B8A8_UNORM", 4, 8, UNorm8_type },
	{ DXGI_FORMAT_R8G8_UNORM           , "R8G8_UNORM", 2, 8, UNorm8_type },
	{ DXGI_FORMAT_R8_UNORM             , "R8_UNORM", 1, 8, UNorm8_type },
	{ DXGI_FORMAT_R8G8B8A8_TYPELESS    , "R8G8B8A8_TYPELESS", 4, 8, Typeless8_type },
	{ DXGI_FORMAT_R8G8_TYPELESS        , "R8G8_TYPELESS", 2, 8, Typeless8_type },
	{ DXGI_FORMAT_R8_TYPELESS          , "R8_TYPELESS", 1, 8, Typeless8_type }
};

//////////////////////////////////////////////////////////////////////

string InputField::GetTypeName() const
{
	string suffix = "";
	if(arraySize > 0)
	{
		suffix = Format("x%d", arraySize + 1);
	}
	return Format("%s%d%s", GetFrom(StorageTypeName, storageType), elementCount, suffix.c_str());
}

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

DXGI_FormatDescriptor *GetDXGIDescriptorFromName(char const *name)
{
	for(uint i = 0; i < _countof(DXGI_Lookup); ++i)
	{
		DXGI_FormatDescriptor &d = DXGI_Lookup[i];
		if(_stricmp(d.name, name) == 0)
		{
			return &d;
		}
	}
	return null;
}

//////////////////////////////////////////////////////////////////////

uint GetStorageSize(DXGI_FORMAT format)
{
	DXGI_FormatDescriptor *d = GetDXGIDescriptor(format);
	return (d != null) ? ((d->bitSize * d->fields) / 8) : 0;
}

//////////////////////////////////////////////////////////////////////

StorageType GetStorageType(DXGI_FORMAT format)
{
	DXGI_FormatDescriptor *d = GetDXGIDescriptor(format);
	return (d != null) ? d->storageType : Invalid_type;
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
	DXGI_FormatDescriptor *f = GetDXGIDescriptor(format);
	return (f != null) ? f->name : "UNKNOWN";
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

StorageType StorageTypeFromName(char const *name)
{
	for(int i = 0; i < _countof(type_suffix); ++i)
	{
		if(_stricmp(name, type_suffix[i].name) == 0)
		{
			return type_suffix[i].storageType;
		}
	}
	return Invalid_type;
}

//////////////////////////////////////////////////////////////////////

Binding *HLSLShader::CreateBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	BindingInfo *info = GetBindingInfo(desc.Type);
	if(info != null)
	{
		switch(desc.Type)
		{
			case D3D_SIT_CBUFFER:
				mConstantBufferBindings.push_back(new ConstantBufferBinding(this, desc));
				++mConstBuffers;
				return mConstantBufferBindings.back();

			case D3D_SIT_TEXTURE:
				mResourceBindings.push_back(new ResourceBinding(this, desc));
				++mResources;
				return mResourceBindings.back();

			case D3D_SIT_SAMPLER:
				mSamplerBindings.push_back(new SamplerBinding(this, desc));
				++mSamplers;
				return mSamplerBindings.back();

			case D3D_SIT_TBUFFER:
				break;
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
	}
	return new Binding(this, desc);
}

//////////////////////////////////////////////////////////////////////
// used for binding to shaders (Samplers, Resources, ConstantBuffers & VertexBuffers)

void AddToBindingRun(vector<HLSLShader::BindingRun> &runs, uint bindPoint)
{
	bool startRun = false;
	if(runs.empty())
	{
		// list is empty, so yes
		startRun = true;
	}
	else
	{
		HLSLShader::BindingRun &currentRun = runs.back();
		if(bindPoint != currentRun.mBindPoint + currentRun.mBindCount)
		{
			// noncontiguous bindpoint, so yes
			startRun = true;
		}
	}
	if(startRun)
	{
		// start a new run
		runs.push_back({ bindPoint, 1 });
	}
	else
	{
		// or just tag onto the current last one
		runs.back().mBindCount++;
	}
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::AddBinding(Binding *b)
{
	BindingInfo *info = GetBindingInfo(b->mDesc.Type);
	BindingInfo::Type type = info->BindingType;
	AddToBindingRun(mBindingRuns[type], b->mDesc.BindPoint);
}

//////////////////////////////////////////////////////////////////////

HRESULT HLSLShader::CreateBindings()
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

		// Create a definition if necessary
		TypeDefinition *def = new TypeDefinition(mReflector, mConstBuffers);
		mDefinitionIDs[def->mDesc.Name] = (uint)mDefinitions.size();
		mDefinitions.push_back(def);
		++mConstBuffers;
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
		if(d.Type == D3D_SIT_SAMPLER)
		{
			D3D11_SHADER_VARIABLE_DESC desc;
			ID3D11ShaderReflectionVariable *v = mReflector->GetVariableByName(d.Name);
			if(SUCCEEDED(v->GetDesc(&desc)))
			{
				TRACE("%s\n", desc.Name);
			}
		}
	}

	delete[] buffers;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////

HLSLShader::HLSLShader(tstring const &filename)
	: mStartIndex(0)
	, mName(StringFromTString(filename))
	, mConstBuffers(0)
	, mSamplers(0)
	, mResources(0)
{
}

//////////////////////////////////////////////////////////////////////

HLSLShader::~HLSLShader()
{
	Destroy();
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputBlob()
{
	OutputCommentLine("Data for %s", Name().c_str());
	OutputLine("uint32 WEAKSYM %s_Data[] =", Name().c_str());
	Indent("{");

	char *sep = "";
	uint32 *d = (uint32 *)mBlob;
	for(uint i = 0; i < mSize / 4; ++i)
	{
		Output(sep);
		if((i & 7) == 0)
		{
			OutputLine();
			OutputIndent();
		}
		Output("0x%08x", d[i]);
		sep = ",";
	}
	OutputLine();
	UnIndent("};");
	OutputLine();
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputConstBufferNamesAndOffsets()
{
	if(!mDefinitions.empty())
	{
		OutputCommentLine("offsets and defaults");
		for(auto i = mDefinitions.begin(); i != mDefinitions.end(); ++i)
		{
			(*i)->StaticsOutput(Name());
		}
		// output the constbuffernames table
		OutputCommentLine("const buffer names table");
		OutputLine("extern char const WEAKSYM *%s_ConstBufferNames[] =", Name().c_str());
		OutputIndent("{");
		Indent();
		char const *sep = "";
		for(auto i = mDefinitions.begin(); i != mDefinitions.end(); ++i)
		{
			Output(sep);
			OutputLine();
			OutputIndent();
			Output("\"%s\"", (*i)->Name);
			sep = ",";
		}
		OutputLine();
		UnIndent("};");
		OutputLine();
	}
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputResourceNames()
{
	if(mResources > 0)
	{
		OutputCommentLine("Texture names");
		OutputLine("extern char const WEAKSYM * %s_TextureNames[] =", Name().c_str());
		OutputIndent("{");
		OutputLine();
		Indent();
		for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
		{
			char const *sep = "";
			if((*i)->IsResource())
			{
				Output(sep);
				OutputLine("\"%s\"", (*i)->Name());
				sep = ",";
			}
		}
		UnIndent("};");
		OutputLine();
	}
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputSamplerNames()
{
	if(mSamplers > 0)
	{
		OutputCommentLine("Sampler names");
		OutputLine("extern char const WEAKSYM * %s_SamplerNames[] =", Name().c_str());
		OutputIndent("{");
		OutputLine();
		Indent();
		for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
		{
			char *sep = "";
			if((*i)->IsSampler())
			{
				Output(sep);
				OutputLine("\"%s\"", (*i)->Name());
				sep = ",";
			}
		}
		UnIndent("};");
		OutputLine();
	}
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputConstBufferMembers()
{
	if(mDefinitions.empty())
	{
		return;
	}

	OutputComment("Const Buffers");
	int index = 0;
	for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	{
		if((*i)->IsConstBuffer())
		{
			(*i)->MemberOutput();
		}
	}
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputSamplerMembers()
{
	if(mSamplers > 0)
	{
		OutputComment("Samplers");
		OutputLine("union");
		OutputLine("{");
		Indent();
		OutputLine("struct");
		OutputLine("{");
		Indent();
		for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
		{
			if((*i)->IsSampler())
			{
				(*i)->MemberOutput();
			}
		}
		UnIndent("};");
		OutputLine("Sampler *samplers[%d];", mResources);
		UnIndent("};");
		OutputLine();
	}
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputResourceMembers()
{
	if(mResources > 0)
	{
		OutputComment("Textures");
		OutputLine("union");
		OutputLine("{");
		Indent();
		OutputLine("struct");
		OutputLine("{");
		Indent();

		for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
		{
			if((*i)->IsResource())
			{
				(*i)->MemberOutput();
			}
		}
		UnIndent("};");
		OutputLine("Texture *textures[%d];", mResources);
		UnIndent("};");
		OutputLine();
	}
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputBindingRuns()
{
	char const *sep = "";
	for(uint i = 0; i < BindingInfo::Type::NumBindingTypes; ++i)
	{
		vector<HLSLShader::BindingRun> &runs = mBindingRuns[i];

		Output("%s{", sep);
		if(!runs.empty())
		{
			Output("{");
			char const *sep2 = "";
			for(auto &run : runs)
			{
				Output("%s{%d,%d}", sep2, run.mBindPoint, run.mBindCount);
				sep2 = ",";
			}
			Output("}");
		}
		Output("}");

		sep = ", ";
	}
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputVertexBufferBindingRuns()
{
	vector<HLSLShader::BindingRun> &runs = mVertexBufferBindingRuns;

	if(!runs.empty())
	{
		OutputLine("extern Shader::BindRun WEAKSYM %s_VertexBufferBindings[%d] = {", mName.c_str(), runs.size());
		Indent();
		OutputIndent();
		char const *sep2 = "";
		for(auto &run : runs)
		{
			Output("%s{%d,%d}", sep2, run.mBindPoint, run.mBindCount);
			sep2 = ",";
		}
	}
	OutputLine();
	UnIndent();
	OutputLine("};");
	OutputLine();
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputConstructor()
{
	string constBufferNames = (mConstantBufferBindings.size() > 0) ? Format("%s_ConstBufferNames", Name().c_str()) : "null";
	string textureNames = (mResourceBindings.size() > 0) ? Format("%s_TextureNames", Name().c_str()) : "null";
	string samplerNames = (mSamplerBindings.size() > 0) ? Format("%s_SamplerNames", Name().c_str()) : "null";

	string vb_bindings;
	if(mShaderTypeDesc.type == ShaderType::Vertex)
	{
		vb_bindings = Format(", %s_VertexBufferBindings, %d", mName.c_str(), mVertexBufferBindingRuns.size());
	}

	OutputComment("Constructor");
	OutputLine("%s()", RefName().c_str());
	Indent();
	OutputLine(": %sShader(%d, %s, %d, %s, %d, %s, %s, %s, %s%s)",
			   ShaderTypeName(),
				mConstantBufferBindings.size(), constBufferNames.c_str(),
				mSamplerBindings.size(), samplerNames.c_str(),
				mResourceBindings.size(), textureNames.c_str(),
				mResourceBindings.size() > 0 ? "textures" : "null",
				mSamplerBindings.size() > 0 ? "samplers" : "null",
				Format("%s_Bindings[ShaderType::%s]", mName.c_str(), mShaderTypeDesc.name).c_str(),
				vb_bindings.c_str()
				);

	for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	{
		(*i)->ConstructorOutput();
	}
	UnIndent("{");
	OutputLine("}");
}

//////////////////////////////////////////////////////////////////////

string HLSLShader::VSTag()
{
	return (mShaderTypeDesc.type == ShaderType::Vertex) ?
		Format(", %s_InputElements, _countof(%s_InputElements)", Name().c_str(), Name().c_str()) :
		string();
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputShaderStruct()
{
	OutputInputStruct();

	OutputCommentLine("%s Shader", mShaderTypeDesc.name);

	OutputLine("struct %s : %sShader", RefName().c_str(), mShaderTypeDesc.name);
	OutputLine("{");
	Indent();

	OutputConstBufferMembers();
	OutputSamplerMembers();
	OutputResourceMembers();
	OutputConstructor();

	UnIndent("};");
	OutputLine();
}

//////////////////////////////////////////////////////////////////////
// still need to sort out the offsets

void HLSLShader::OutputInputElements()
{
	if(mShaderTypeDesc.type != ShaderType::Vertex)
	{
		return;
	}

	OutputCommentLine("Input Element descs");
	OutputLine("extern D3D11_INPUT_ELEMENT_DESC const WEAKSYM %s_InputElements[%d] =", Name().c_str(), mInputElements.size());
	OutputIndent();
	Output("{");
	Indent();
	char const *sep = "";
	uint offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
	for(uint i = 0; i < mInputElements.size(); ++i)
	{
		D3D11_INPUT_ELEMENT_DESC &d = mInputElements[i];
		char const *step;
		if(d.InstanceDataStepRate == 0)
		{
			step = "D3D11_INPUT_PER_VERTEX_DATA";
		}
		else
		{
			step = "D3D11_INPUT_PER_INSTANCE_DATA";
		}
		char const *formatName = GetFormatName(d.Format);
		Output(sep);
		OutputLine();
		OutputIndent();
		Output("{ \"%s\", %u, DXGI_FORMAT_%s, %d, %d, %s, %d }", d.SemanticName, d.SemanticIndex, formatName, d.InputSlot, offsets[d.InputSlot], step, d.InstanceDataStepRate);
		offsets[d.InputSlot] += GetStorageSize(d.Format);
		sep = ",";
	}
	OutputLine();
	UnIndent("};");
	OutputLine();
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputMemberVariable()
{
	OutputLine("%s %s;", mShaderTypeDesc.refName, ToLower(mShaderTypeDesc.refName).c_str());
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputInputStruct()
{
	if(mShaderTypeDesc.type != ShaderType::Vertex)
	{
		return;
	}

	// Output an InputVertex# for each stream
	// Typedef InputVertex to InputVertex0

	OutputCommentLine("InputVertex");

	uint maxStream = 0;
	for(auto &i : mInputFields)
	{
		maxStream = max(maxStream, i.stream);
	}

	for(uint s = 0; s <= maxStream; ++s)
	{
		bool header = false;
		for(auto &f : mInputFields)
		{
			if(f.stream == s)
			{
				if(!header)
				{
					OutputLine("struct InputVertex%d", s);
					OutputLine("{");
					Indent();
					header = true;
				}
				OutputLine("%s;", f.GetDeclaration().c_str());
			}
		}
		if(header)
		{
			UnIndent("};");
			OutputLine();
			OutputLine("using VertBuffer%d = VertexBuffer<InputVertex%d>;", s, s);
			OutputLine();
		}
	}
	OutputLine("using InputVertex = InputVertex0;");
	OutputLine("using VertBuffer = VertexBuffer<InputVertex>;");
	OutputLine();
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputHeaderFile()
{
	Printer::SetShader(this);
	OutputShaderStruct();
}

//////////////////////////////////////////////////////////////////////

bool IsAllDigits(string const &str)
{
	for(auto c : str)
	{
		if(!isdigit(c))
		{
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
// Need to create a bindlist for the vertex streams...

using std::regex;

static regex underscore_tokenizer("([A-Za-z0-9]+)_?");

HRESULT HLSLShader::CreateInputLayout()
{
	int n = mShaderDesc.InputParameters;
	mInputElements.resize(n);
	mInputFields.resize(0);
	int vertexSize = 0;
	for(int i = 0; i < n; ++i)
	{
		InputField f;
		f.arraySize = 0;
		D3D11_SIGNATURE_PARAMETER_DESC desc;
		DXR(mReflector->GetInputParameterDesc(i, &desc));
		D3D11_INPUT_ELEMENT_DESC &d = mInputElements[i];
		d.SemanticName = desc.SemanticName;
		d.SemanticIndex = desc.SemanticIndex;
		d.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		d.Format = DXGI_FORMAT_UNKNOWN;

		int sourceFields = CountBits(desc.Mask);

		// if they ask, try to find the right storage format
		int fieldCount = 0;
		StorageType storageType = StorageType::Invalid_type;
		TRACE("Semantic name: %s\n", d.SemanticName);

		// if it has underscores in the semantic name
		// and there are exactly 3 underscores
		// and regex returns 4 matches, each with 1 submatch
		// and the 2nd and 3rd matches[1] are integers
		// then it's probably a type_stream_instances_name declaration
		// so treat it as such

		// if there is more than 1 stream
		// define multiple InputVertex structures
		// struct InputVertex0-N { ... };

		// if instances == V, then use D3D11_INPUT_PER_VERTEX_DATA
		// else use D3D11_INPUT_PER_INSTANCE_DATA

		// get everything up to the last _
		string type_annotation;
		string semantic_name = d.SemanticName;
		string stream;
		string instances;

		uint streamID = 0;
		uint instanceDataStepRate = 0;

		// recognize this semantic (eg was a semantic:() declaration used)?
		auto &semanticIter = semantics.find(d.SemanticName);
		if(semanticIter != semantics.end())
		{
			// yes
			Semantic &s = semanticIter->second;

			// get & check the stream
			streamID = s.stream;
			if(streamID >= D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT)
			{
				emit_error("Invalid input resource slot");
				return E_FAIL;
			}

			// and instance rate
			instanceDataStepRate = s.instanced ? s.instances : 0;

			if(s.nativeFormat != null)
			{
				d.Format = s.nativeFormat->format;
				fieldCount = s.nativeFormat->fields;
				storageType = s.nativeFormat->storageType;
			}
			else
			{
				storageType = s.type;
				fieldCount = sourceFields;

				if(s.type != Invalid_type)
				{
					d.Format = GetDXGI_Format(sourceFields, s.type);
				}
			}
		}

		// got a format yet?
		if(d.Format == DXGI_FORMAT_UNKNOWN)
		{
			// no, use a simple mapping
			fieldCount = sourceFields;
			d.Format = formats[fieldCount - 1][desc.ComponentType];
			storageType = GetStorageType(d.Format);
		}

		if(fieldCount != sourceFields)
		{
			emit_error("Incompatible semantic type specified");
			return E_INVALIDARG;
		}

		vertexSize += SizeOfFormatElement(d.Format) / 8;
		f.storageType = storageType;
		f.elementCount = fieldCount;
		f.varName = semantic_name;
		f.stream = streamID;
		f.instances = instanceDataStepRate;

		d.InputSlot = f.stream;
		d.InputSlotClass = (f.instances == 0) ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
		d.InstanceDataStepRate = f.instances;

		// is it identical to the last one but with a +1 semantic index?
		// should do this check at the start...
		D3D11_INPUT_ELEMENT_DESC *pe = &mInputElements[i];
		--pe;
		if(	mInputFields.size() > 0 &&
			strcmp(d.SemanticName, pe->SemanticName) == 0 &&
			d.Format == pe->Format &&
			d.SemanticIndex == (pe->SemanticIndex + 1) &&
			d.InputSlot == pe->InputSlot &&
			d.InputSlotClass == pe->InputSlotClass && 
			d.InstanceDataStepRate == pe->InstanceDataStepRate)
		{
			// ha! probably part of a matrix of some sort
			// increment the mArraySize of f and don't create a new one...
			InputField &p = mInputFields.back();
			p.arraySize++;
		}
		else
		{
			mInputFields.push_back(f);
		}
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

HRESULT HLSLShader::CreateVertexBufferBindingRuns()
{
	// reet
	// scan the InputLayout for streams
	// build a Set<> of the streams
	// for each stream
	//   add to binding run

	std::set<int> streams;

	for(auto &i : mInputFields)
	{
		streams.emplace(i.stream);
	}

	for(auto &s : streams)
	{
		AddToBindingRun(mVertexBufferBindingRuns, s);
	}

	for(auto &r : mVertexBufferBindingRuns)
	{
		Trace("Run at %d, length = %d\n", r.mBindPoint, r.mBindCount);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////

HRESULT HLSLShader::Create(void const *blob, size_t size, ShaderTypeDesc const &desc)
{
	mShaderType = desc.type;
	mBlob = blob;
	mSize = size;
	mShaderTypeDesc = desc;
	DXR(D3DReflect(blob, size, IID_ID3D11ShaderReflection, (void **)&mReflector));
	mReflector->GetDesc(&mShaderDesc);
	DXR(CreateBindings());
	if(mShaderTypeDesc.type == ShaderType::Vertex)
	{
		DXR(CreateInputLayout());
		DXR(CreateVertexBufferBindingRuns());
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

HRESULT HLSLShader::Destroy()
{
	mDefinitions.clear();
	mDefinitionIDs.clear();
	mConstantBufferBindings.clear();
	mResourceBindings.clear();
	mSamplerBindings.clear();
	mTextureBufferIDs.clear();
	mConstBufferIDs.clear();
	mSamplerIDs.clear();
	mTextureIDs.clear();
	mName.clear();
	mBindings.clear();
	mReflector.Release();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

char const *HLSLShader::ShaderTypeName() const
{
	return mShaderTypeDesc.name;
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::Dump()
{
}