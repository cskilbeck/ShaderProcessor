//////////////////////////////////////////////////////////////////////
// Make it a proper Compile type in VS2013
// Sort out Resource types (Texture1D/2D/3D etc)
// Support instancing?
// Support all shader types
// Support compute
// support Buffers<>
// deal with anonymous cbuffers and tbuffers (index instead of name)
// test alignment/padding etc
// deal with Buffers of structs (no padding)
// Allow row or column major matrices

#include "stdafx.h"

using Printer::output;

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
	"Float"     , 0, Float_type,
	"Half"      , 0, Half_type,
	"Int"       , 0, Int_type,
	"UInt"      , 0, UInt_type,
	"Short"     , 0, Short_type,
	"UShort"    , 0, UShort_type,
	"Int8"      , 0, SByte_type,
	"SByte"     , 0, SByte_type,
	"Byte"      , 0, UNorm8_type,
	"UInt8"     , 0, Byte_type,
	"Norm16"    , 0, Norm16_type,
	"Norm8"     , 0, Norm8_type,
	"UNorm16"   , 0, UNorm16_type,
	"UNorm8"    , 0, UNorm8_type,
	"Typeless32", 0, Typeless32_type,
	"Typeless16", 0, Typeless16_type,
	"Typeless8" , 0, Typeless8_type
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
	return Format("%s%d", GetFrom(StorageTypeName, storageType), elementCount);
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
	return (f != null) ? f->name : "DXGI_FORMAT_UNKNOWN";
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

Binding *HLSLShader::CreateBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	BindingInfo *info = GetBindingInfo(desc.Type);
	if(info != null)
	{
		char const *bindingTypeName = GetBindingTypeName(info->BindingType);
		TRACE("%s binding %d is a %s called %s\n", bindingTypeName, desc.BindPoint, info->Name, desc.Name);
		switch(desc.Type)
		{
			case D3D_SIT_CBUFFER:
				++mConstBuffers;
				return new ConstantBufferBinding(this, desc);
			case D3D_SIT_TBUFFER:
				break;
			case D3D_SIT_TEXTURE:
				++mResources;
				mResourceBindings.push_back(new ResourceBinding(this, desc));
				return mResourceBindings.back();
			case D3D_SIT_SAMPLER:
				++mSamplers;
				mSamplerBindings.push_back(new SamplerBinding(this, desc));
				return mSamplerBindings.back();
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

HRESULT HLSLShader::CreateBindings()
{
	Binding::ClearAllBindings();

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

	Binding::ShowAllBindings();

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
	Printer::SetShader(this);
}

//////////////////////////////////////////////////////////////////////

HLSLShader::~HLSLShader()
{
	Destroy();
}

//////////////////////////////////////////////////////////////////////

static string header = "#pragma pack(push, 4)\n\n";
static string footer = "#pragma pack(pop)\n\n";
static char const *comment = "//////////////////////////////////////////////////////////////////////\n";

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputHeader(char const *filename) // static
{
	output(comment);
	output("// %s.h - auto generated file, do not edit\n\n", filename);
	output("#pragma once\n");
	output("%s", header.c_str());
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputFooter() // static
{
	output("%s", footer.c_str());
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputBlob()
{
	output("%s// %s data\n\nuint32 WEAKSYM %s_Data[] =\n{", comment, Name().c_str(), Name().c_str());
	char *sep = "";
	uint32 *d = (uint32 *)mBlob;
	for(uint i = 0; i < mSize / 4; ++i)
	{
		output(sep);
		if((i & 7) == 0)
		{
			output("\n\t");
		}
		output("0x%08x", d[i]);
		sep = ",";
	}
	output("\n};\n\n");
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputConstBufferNamesAndOffsets()
{
	if(!mDefinitions.empty())
	{
		output("%s// offsets and defaults\n\n", comment);
		for(auto i = mDefinitions.begin(); i != mDefinitions.end(); ++i)
		{
			(*i)->StaticsOutput(Name());
		}
		// output the constbuffernames table
		output("%s// const buffer names table\n", comment);
		output("\nextern char const WEAKSYM *%s_ConstBufferNames[] =\n{", Name().c_str());
		char const *sep = "";
		for(auto i = mDefinitions.begin(); i != mDefinitions.end(); ++i)
		{
			output("%s\n\t\"%s\"", sep, (*i)->Name);
			sep = ",";
		}
		output("\n};\n\n");
	}
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputResourceNames()
{
	if(mResources > 0)
	{
		output("%s// Texture names\n\n", comment);
		output("extern char const WEAKSYM * %s_TextureNames[] =\n{", Name().c_str());
		for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
		{
			char *sep = "";
			if((*i)->IsResource())
			{
				output("%s\n\t\"%s\"", sep, (*i)->Name());
			}
		}
		output("\n};\n");
	}

}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputSamplerNames()
{
	if(mSamplers > 0)
	{
		output("%s// Sampler names\n\n", comment);
		output("extern char const WEAKSYM * %s_SamplerNames[] =\n{", Name().c_str());
		for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
		{
			char *sep = "";
			if((*i)->IsSampler())
			{
				output("%s\n\t\"%s\"", sep, (*i)->Name());
			}
		}
		output("\n};\n\n");
	}
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputConstBufferMembers()
{
	for(auto i = mDefinitions.begin(); i != mDefinitions.end(); ++i)
	{
		(*i)->MemberOutput(this->Name());
	}
}

//////////////////////////////////////////////////////////////////////

static char const *unionHeader = "\tunion\n\t{\n\t\tstruct\n\t\t{\n";
static char const *textureLine = "\t\t};\n\t\t%s *%s[%d];\n\t};\n\n";

void HLSLShader::OutputSamplerMembers()
{
	if(mSamplers > 0)
	{
		output(unionHeader);
		for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
		{
			if((*i)->IsSampler())
			{
				output("\t\t\t");
				(*i)->MemberOutput();
				output("\n");
			}
		}
		output(textureLine, "Sampler", "samplers", mSamplers);
	}
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputResourceMembers()
{
	if(mResources > 0)
	{
		output(unionHeader);
		for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
		{
			if((*i)->IsResource())
			{
				output("\t\t\t");
				(*i)->MemberOutput();
				output("\n");
			}
		}
		output(textureLine, "Texture", "textures", mSamplers);
	}
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputConstructor()
{
	string constBufferNames = (mConstBuffers > 0) ? Format("%s_ConstBufferNames", Name().c_str()) : "null";
	string textureNames = (mResources > 0) ? Format("%s_TextureNames", Name().c_str()) : "null";
	string samplerNames = (mSamplers > 0) ? Format("%s_SamplerNames", Name().c_str()) : "null";

	output("\t// Constructor\n\t%s()\n\t\t: %sShader(%s_Data, %d, %d, %s, %d, %s, %d, %s, %s, %s)",
		   Name().c_str(),
		   mShaderTypeDesc.name,
		   Name().c_str(),
		   mSize,
		   mConstBuffers,
		   constBufferNames.c_str(),
		   mSamplers,
		   samplerNames.c_str(),
		   mResources,
		   textureNames.c_str(),
		   mResources > 0 ? "textures" : "null",
		   mSamplers > 0 ? "samplers" : "null"
		   );
	for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	{
		output("\n\t\t, ");
		(*i)->ConstructorOutput();
	}

	output("\n\t{\n\t}\n");
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputShaderStruct()
{
	output("\n%s// %s Shader: %s\n\n", comment, mShaderTypeDesc.name, Name().c_str());

	string vsTag;

	if(mShaderTypeDesc.type == ShaderType::Vertex)
	{
		vsTag = Format("<%s_InputElements, _countof(%s_InputElements)>", Name().c_str(), Name().c_str());
	}

	output("struct %s : %sShader%s, Aligned16\n", Name().c_str(), mShaderTypeDesc.name, vsTag.c_str());
	output("{\n");

	OutputConstBufferMembers();
	OutputSamplerMembers();
	OutputResourceMembers();
	OutputInputStruct();
	OutputConstructor();

	output("};\n\n");
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputInputElements()
{
	if(mShaderTypeDesc.type != ShaderType::Vertex)
	{
		return;
	}

	output("%s// Input Element descs\n\n", comment);
	output("extern D3D11_INPUT_ELEMENT_DESC const WEAKSYM %s_InputElements[%d] =\n{", Name().c_str(), mInputElements.size());
	char const *sep = "";
	for(uint i = 0; i < mInputElements.size(); ++i)
	{
		D3D11_INPUT_ELEMENT_DESC &d = mInputElements[i];
		char const *formatName = GetFormatName(d.Format);
		output("%s\n\t{ \"%s\", %u, DXGI_FORMAT_%s, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }", sep, d.SemanticName, d.SemanticIndex, formatName);
		sep = ",";
	}
	output("\n};\n");
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::OutputInputStruct()
{
	if(mShaderTypeDesc.type != ShaderType::Vertex)
	{
		return;
	}

	output("\tstruct InputVertex\n\t{\n");
	for(uint i = 0; i < mInputElements.size(); ++i)
	{
		InputField &f = mInputFields[i];
		output("\t\t%s;\n", f.GetDeclaration().c_str());
	}
	output("\t};\n\n");
	output("\tusing VertexBuffer = VertexBuffer<InputVertex>;\n\n");
}

//////////////////////////////////////////////////////////////////////

void HLSLShader::Output()
{
	OutputBlob();
	OutputConstBufferNamesAndOffsets();
	OutputSamplerNames();
	OutputResourceNames();
	OutputInputElements();
	OutputShaderStruct();
}

//////////////////////////////////////////////////////////////////////

HRESULT HLSLShader::CreateDefinitions()
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

HRESULT HLSLShader::CreateInputLayout()
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
			for(int j = 0; j < _countof(type_suffix); ++j)
			{
				if(_stricmp(type_annotation.c_str(), type_suffix[j].name) == 0)
				{
					int fc = type_suffix[j].fieldCount;
					fieldCount = (fc == 0) ? sourceFields : fc;
					storageType = type_suffix[j].storageType;
					break;
				}
			}
			if(fieldCount == 0)
			{
				// else see if they want to specify the format explicitly
				for(int j = 0; j < _countof(DXGI_Lookup); ++j)
				{
					if(_stricmp(DXGI_Lookup[j].name, type_annotation.c_str()) == 0)
					{
						fieldCount = DXGI_Lookup[j].fields;
						storageType = DXGI_Lookup[j].storageType;
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
		f.storageType = storageType;
		f.elementCount = fieldCount;
		f.varName = semantic_name;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

HRESULT HLSLShader::Create(void const *blob, size_t size, ShaderTypeDesc const &desc)
{
	mBlob = blob;
	mSize = size;
	mShaderTypeDesc = desc;
	DX(D3DReflect(blob, size, IID_ID3D11ShaderReflection, (void **)&mReflector));
	mReflector->GetDesc(&mShaderDesc);
	DX(CreateDefinitions());
	DX(CreateBindings());
	if(mShaderTypeDesc.type == ShaderType::Vertex)
	{
		DX(CreateInputLayout());
	}
	Output();
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
