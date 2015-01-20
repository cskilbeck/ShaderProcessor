//////////////////////////////////////////////////////////////////////
// Make it so it can compile multiple shader types in one run
// Sort out the semantic name annotation parser & emit InputLayoutDefinitions
// Make it a proper Compile type in VS2013

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

using ISMap = std::map<int, string>;

//////////////////////////////////////////////////////////////////////

static string const &GetFrom(ISMap const &map, int x)
{
	static string const unknown("?");
	auto i = map.find(x);
	return (i != map.end()) ? i->second : unknown;
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

static ISMap shader_input_type_names =
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

static ISMap shader_input_dimension_names =
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

struct FormatName_t
{
	char const *name;
	DXGI_FORMAT format;
};

FormatName_t FormatName[] =
{
	{ "DXGI_FORMAT_UNKNOWN", DXGI_FORMAT_UNKNOWN },
	{ "DXGI_FORMAT_R32G32B32A32_TYPELESS", DXGI_FORMAT_R32G32B32A32_TYPELESS },
	{ "DXGI_FORMAT_R32G32B32A32_FLOAT", DXGI_FORMAT_R32G32B32A32_FLOAT },
	{ "DXGI_FORMAT_R32G32B32A32_UINT", DXGI_FORMAT_R32G32B32A32_UINT },
	{ "DXGI_FORMAT_R32G32B32A32_SINT", DXGI_FORMAT_R32G32B32A32_SINT },
	{ "DXGI_FORMAT_R32G32B32_TYPELESS", DXGI_FORMAT_R32G32B32_TYPELESS },
	{ "DXGI_FORMAT_R32G32B32_FLOAT", DXGI_FORMAT_R32G32B32_FLOAT },
	{ "DXGI_FORMAT_R32G32B32_UINT", DXGI_FORMAT_R32G32B32_UINT },
	{ "DXGI_FORMAT_R32G32B32_SINT", DXGI_FORMAT_R32G32B32_SINT },
	{ "DXGI_FORMAT_R16G16B16A16_TYPELESS", DXGI_FORMAT_R16G16B16A16_TYPELESS },
	{ "DXGI_FORMAT_R16G16B16A16_FLOAT", DXGI_FORMAT_R16G16B16A16_FLOAT },
	{ "DXGI_FORMAT_R16G16B16A16_UNORM", DXGI_FORMAT_R16G16B16A16_UNORM },
	{ "DXGI_FORMAT_R16G16B16A16_UINT", DXGI_FORMAT_R16G16B16A16_UINT },
	{ "DXGI_FORMAT_R16G16B16A16_SNORM", DXGI_FORMAT_R16G16B16A16_SNORM },
	{ "DXGI_FORMAT_R16G16B16A16_SINT", DXGI_FORMAT_R16G16B16A16_SINT },
	{ "DXGI_FORMAT_R32G32_TYPELESS", DXGI_FORMAT_R32G32_TYPELESS },
	{ "DXGI_FORMAT_R32G32_FLOAT", DXGI_FORMAT_R32G32_FLOAT },
	{ "DXGI_FORMAT_R32G32_UINT", DXGI_FORMAT_R32G32_UINT },
	{ "DXGI_FORMAT_R32G32_SINT", DXGI_FORMAT_R32G32_SINT },
	{ "DXGI_FORMAT_R32G8X24_TYPELESS", DXGI_FORMAT_R32G8X24_TYPELESS },
	{ "DXGI_FORMAT_D32_FLOAT_S8X24_UINT", DXGI_FORMAT_D32_FLOAT_S8X24_UINT },
	{ "DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS", DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS },
	{ "DXGI_FORMAT_X32_TYPELESS_G8X24_UINT", DXGI_FORMAT_X32_TYPELESS_G8X24_UINT },
	{ "DXGI_FORMAT_R10G10B10A2_TYPELESS", DXGI_FORMAT_R10G10B10A2_TYPELESS },
	{ "DXGI_FORMAT_R10G10B10A2_UNORM", DXGI_FORMAT_R10G10B10A2_UNORM },
	{ "DXGI_FORMAT_R10G10B10A2_UINT", DXGI_FORMAT_R10G10B10A2_UINT },
	{ "DXGI_FORMAT_R11G11B10_FLOAT", DXGI_FORMAT_R11G11B10_FLOAT },
	{ "DXGI_FORMAT_R8G8B8A8_TYPELESS", DXGI_FORMAT_R8G8B8A8_TYPELESS },
	{ "DXGI_FORMAT_R8G8B8A8_UNORM", DXGI_FORMAT_R8G8B8A8_UNORM },
	{ "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB },
	{ "DXGI_FORMAT_R8G8B8A8_UINT", DXGI_FORMAT_R8G8B8A8_UINT },
	{ "DXGI_FORMAT_R8G8B8A8_SNORM", DXGI_FORMAT_R8G8B8A8_SNORM },
	{ "DXGI_FORMAT_R8G8B8A8_SINT", DXGI_FORMAT_R8G8B8A8_SINT },
	{ "DXGI_FORMAT_R16G16_TYPELESS", DXGI_FORMAT_R16G16_TYPELESS },
	{ "DXGI_FORMAT_R16G16_FLOAT", DXGI_FORMAT_R16G16_FLOAT },
	{ "DXGI_FORMAT_R16G16_UNORM", DXGI_FORMAT_R16G16_UNORM },
	{ "DXGI_FORMAT_R16G16_UINT", DXGI_FORMAT_R16G16_UINT },
	{ "DXGI_FORMAT_R16G16_SNORM", DXGI_FORMAT_R16G16_SNORM },
	{ "DXGI_FORMAT_R16G16_SINT", DXGI_FORMAT_R16G16_SINT },
	{ "DXGI_FORMAT_R32_TYPELESS", DXGI_FORMAT_R32_TYPELESS },
	{ "DXGI_FORMAT_D32_FLOAT", DXGI_FORMAT_D32_FLOAT },
	{ "DXGI_FORMAT_R32_FLOAT", DXGI_FORMAT_R32_FLOAT },
	{ "DXGI_FORMAT_R32_UINT", DXGI_FORMAT_R32_UINT },
	{ "DXGI_FORMAT_R32_SINT", DXGI_FORMAT_R32_SINT },
	{ "DXGI_FORMAT_R24G8_TYPELESS", DXGI_FORMAT_R24G8_TYPELESS },
	{ "DXGI_FORMAT_D24_UNORM_S8_UINT", DXGI_FORMAT_D24_UNORM_S8_UINT },
	{ "DXGI_FORMAT_R24_UNORM_X8_TYPELESS", DXGI_FORMAT_R24_UNORM_X8_TYPELESS },
	{ "DXGI_FORMAT_X24_TYPELESS_G8_UINT", DXGI_FORMAT_X24_TYPELESS_G8_UINT },
	{ "DXGI_FORMAT_R8G8_TYPELESS", DXGI_FORMAT_R8G8_TYPELESS },
	{ "DXGI_FORMAT_R8G8_UNORM", DXGI_FORMAT_R8G8_UNORM },
	{ "DXGI_FORMAT_R8G8_UINT", DXGI_FORMAT_R8G8_UINT },
	{ "DXGI_FORMAT_R8G8_SNORM", DXGI_FORMAT_R8G8_SNORM },
	{ "DXGI_FORMAT_R8G8_SINT", DXGI_FORMAT_R8G8_SINT },
	{ "DXGI_FORMAT_R16_TYPELESS", DXGI_FORMAT_R16_TYPELESS },
	{ "DXGI_FORMAT_R16_FLOAT", DXGI_FORMAT_R16_FLOAT },
	{ "DXGI_FORMAT_D16_UNORM", DXGI_FORMAT_D16_UNORM },
	{ "DXGI_FORMAT_R16_UNORM", DXGI_FORMAT_R16_UNORM },
	{ "DXGI_FORMAT_R16_UINT", DXGI_FORMAT_R16_UINT },
	{ "DXGI_FORMAT_R16_SNORM", DXGI_FORMAT_R16_SNORM },
	{ "DXGI_FORMAT_R16_SINT", DXGI_FORMAT_R16_SINT },
	{ "DXGI_FORMAT_R8_TYPELESS", DXGI_FORMAT_R8_TYPELESS },
	{ "DXGI_FORMAT_R8_UNORM", DXGI_FORMAT_R8_UNORM },
	{ "DXGI_FORMAT_R8_UINT", DXGI_FORMAT_R8_UINT },
	{ "DXGI_FORMAT_R8_SNORM", DXGI_FORMAT_R8_SNORM },
	{ "DXGI_FORMAT_R8_SINT", DXGI_FORMAT_R8_SINT },
	{ "DXGI_FORMAT_A8_UNORM", DXGI_FORMAT_A8_UNORM },
	{ "DXGI_FORMAT_R1_UNORM", DXGI_FORMAT_R1_UNORM },
	{ "DXGI_FORMAT_R9G9B9E5_SHAREDEXP", DXGI_FORMAT_R9G9B9E5_SHAREDEXP },
	{ "DXGI_FORMAT_R8G8_B8G8_UNORM", DXGI_FORMAT_R8G8_B8G8_UNORM },
	{ "DXGI_FORMAT_G8R8_G8B8_UNORM", DXGI_FORMAT_G8R8_G8B8_UNORM },
	{ "DXGI_FORMAT_BC1_TYPELESS", DXGI_FORMAT_BC1_TYPELESS },
	{ "DXGI_FORMAT_BC1_UNORM", DXGI_FORMAT_BC1_UNORM },
	{ "DXGI_FORMAT_BC1_UNORM_SRGB", DXGI_FORMAT_BC1_UNORM_SRGB },
	{ "DXGI_FORMAT_BC2_TYPELESS", DXGI_FORMAT_BC2_TYPELESS },
	{ "DXGI_FORMAT_BC2_UNORM", DXGI_FORMAT_BC2_UNORM },
	{ "DXGI_FORMAT_BC2_UNORM_SRGB", DXGI_FORMAT_BC2_UNORM_SRGB },
	{ "DXGI_FORMAT_BC3_TYPELESS", DXGI_FORMAT_BC3_TYPELESS },
	{ "DXGI_FORMAT_BC3_UNORM", DXGI_FORMAT_BC3_UNORM },
	{ "DXGI_FORMAT_BC3_UNORM_SRGB", DXGI_FORMAT_BC3_UNORM_SRGB },
	{ "DXGI_FORMAT_BC4_TYPELESS", DXGI_FORMAT_BC4_TYPELESS },
	{ "DXGI_FORMAT_BC4_UNORM", DXGI_FORMAT_BC4_UNORM },
	{ "DXGI_FORMAT_BC4_SNORM", DXGI_FORMAT_BC4_SNORM },
	{ "DXGI_FORMAT_BC5_TYPELESS", DXGI_FORMAT_BC5_TYPELESS },
	{ "DXGI_FORMAT_BC5_UNORM", DXGI_FORMAT_BC5_UNORM },
	{ "DXGI_FORMAT_BC5_SNORM", DXGI_FORMAT_BC5_SNORM },
	{ "DXGI_FORMAT_B5G6R5_UNORM", DXGI_FORMAT_B5G6R5_UNORM },
	{ "DXGI_FORMAT_B5G5R5A1_UNORM", DXGI_FORMAT_B5G5R5A1_UNORM },
	{ "DXGI_FORMAT_B8G8R8A8_UNORM", DXGI_FORMAT_B8G8R8A8_UNORM },
	{ "DXGI_FORMAT_B8G8R8X8_UNORM", DXGI_FORMAT_B8G8R8X8_UNORM },
	{ "DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM", DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM },
	{ "DXGI_FORMAT_B8G8R8A8_TYPELESS", DXGI_FORMAT_B8G8R8A8_TYPELESS },
	{ "DXGI_FORMAT_B8G8R8A8_UNORM_SRGB", DXGI_FORMAT_B8G8R8A8_UNORM_SRGB },
	{ "DXGI_FORMAT_B8G8R8X8_TYPELESS", DXGI_FORMAT_B8G8R8X8_TYPELESS },
	{ "DXGI_FORMAT_B8G8R8X8_UNORM_SRGB", DXGI_FORMAT_B8G8R8X8_UNORM_SRGB },
	{ "DXGI_FORMAT_BC6H_TYPELESS", DXGI_FORMAT_BC6H_TYPELESS },
	{ "DXGI_FORMAT_BC6H_UF16", DXGI_FORMAT_BC6H_UF16 },
	{ "DXGI_FORMAT_BC6H_SF16", DXGI_FORMAT_BC6H_SF16 },
	{ "DXGI_FORMAT_BC7_TYPELESS", DXGI_FORMAT_BC7_TYPELESS },
	{ "DXGI_FORMAT_BC7_UNORM", DXGI_FORMAT_BC7_UNORM },
	{ "DXGI_FORMAT_BC7_UNORM_SRGB", DXGI_FORMAT_BC7_UNORM_SRGB },
	{ "DXGI_FORMAT_AYUV", DXGI_FORMAT_AYUV },
	{ "DXGI_FORMAT_Y410", DXGI_FORMAT_Y410 },
	{ "DXGI_FORMAT_Y416", DXGI_FORMAT_Y416 },
	{ "DXGI_FORMAT_NV12", DXGI_FORMAT_NV12 },
	{ "DXGI_FORMAT_P010", DXGI_FORMAT_P010 },
	{ "DXGI_FORMAT_P016", DXGI_FORMAT_P016 },
	{ "DXGI_FORMAT_420_OPAQUE", DXGI_FORMAT_420_OPAQUE },
	{ "DXGI_FORMAT_YUY2", DXGI_FORMAT_YUY2 },
	{ "DXGI_FORMAT_Y210", DXGI_FORMAT_Y210 },
	{ "DXGI_FORMAT_Y216", DXGI_FORMAT_Y216 },
	{ "DXGI_FORMAT_NV11", DXGI_FORMAT_NV11 },
	{ "DXGI_FORMAT_AI44", DXGI_FORMAT_AI44 },
	{ "DXGI_FORMAT_IA44", DXGI_FORMAT_IA44 },
	{ "DXGI_FORMAT_P8", DXGI_FORMAT_P8 },
	{ "DXGI_FORMAT_A8P8", DXGI_FORMAT_A8P8 },
	{ "DXGI_FORMAT_B4G4R4A4_UNORM", DXGI_FORMAT_B4G4R4A4_UNORM }
};

char const *GetFormatName(DXGI_FORMAT format)
{
	for(uint i = 0; i < _countof(FormatName); ++i)
	{
		if(FormatName[i].format == format)
		{
			return FormatName[i].name;
		}
	}
	return "DXGI_FORMAT_UNKNOWN";
}

//////////////////////////////////////////////////////////////////////

Binding *Shader::CreateBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	TRACE("Binding %s at %d is a %s called %s\n", GetFrom(shader_input_type_names, desc.Type).c_str(), desc.BindPoint, GetFrom(shader_input_type_names, desc.Type).c_str(), desc.Name);
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
		TRACE("Constant buffer %d (%s) is %s\n", i, buffer->Name, GetFrom(constant_buffer_type_names, buffer->Type).c_str());
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

	// output vertex structure for input

	printf(constructor.c_str(), Name().c_str());
	char const *sep = ": ";
	for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	{
		printf("\n\t\t\t%s", sep);
		(*i)->ConstructorOutput();
		sep = ", ";
	}

	printf("\t\t{\n\t\t}\n");
	printf("\t};\n\n\tDECLSPEC_SELECTANY %s_t %s;\n", Name().c_str(), Name().c_str());

	printf("%s", footer.c_str());
}

//////////////////////////////////////////////////////////////////////
//LPCSTR                     %s                           SemanticName;
//UINT                       %u                           SemanticIndex;
//DXGI_FORMAT                %s                           Format;
//UINT                       0                            InputSlot;
//UINT                       D3D11_APPEND_ALIGNED_ELEMENT AlignedByteOffset;
//D3D11_INPUT_CLASSIFICATION D3D11_INPUT_PER_VERTEX_DATA  InputSlotClass;
//UINT                       0                            InstanceDataStepRate;

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

enum StorageType
{
	Float32,
	Float16,
	Int32,
	Int16,
	Int8,
	UInt32,
	UInt16,
	UInt8,
	Norm32,
	Norm16,
	Norm8,
	UNorm32,
	UNorm16,
	UNorm8,
	Typeless32,
	Typeless16,
	Typeless8
};

enum StorageClass
{
	Float,
	Int,
	UInt,
	Norm,
	UNorm,
	Typeless
};

//////////////////////////////////////////////////////////////////////
// Missing some wacky short float types...


uint StorageSize[] =
{
	32,		// Float32,
	16,		// Float16,
	32,		// Int32,
	16,		// Int16,
	8,		// Int8,
	32,		// UInt32,
	16,		// UInt16,
	8,		// UInt8,
	32,		// Norm32,
	16,		// Norm16,
	8,		// Norm8,
	32,		// UNorm32,
	16,		// UNorm16,
	8,		// UNorm8
	32,		// Typeless32,
	16,		// Typeless16,
	8		// Typeless8,
};

struct InputType
{
	char const *name;
	int fieldCount;	// if this is 0, infer from input type
	StorageType storageType;
};

InputType type_suffix[] =
{
	"BYTE", 0, UNorm8,
	"FLOAT", 0, Float32,
	"FLOAT32", 0, Float32,
	"FLOAT16", 0, Float16,
	"INT32", 0, Int32,
	"INT16", 0, Int16,
	"INT8", 0, Int8,
	"UINT32", 0, UInt32,
	"UINT16", 0, UInt16,
	"UINT8", 0, UInt8,
	"NORM32", 0, Norm32,
	"NORM16", 0, Norm16,
	"NORM8", 0, Norm8,
	"UNORM32", 0, UNorm32,
	"UNORM16", 0, UNorm16,
	"UNORM8", 0, UNorm8,
	"TYPELESS32", 0, Typeless32,
	"TYPELESS16", 0, Typeless16,
	"TYPELESS8", 0, Typeless8
};

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

struct DXGI_Lookup_t
{
	DXGI_FORMAT format;
	uint fields;
	StorageType storageType;
};

DXGI_Lookup_t DXGI_Lookup[] =
{
	// 32
	{ DXGI_FORMAT_R32G32B32A32_FLOAT, 4, Float32 },
	{ DXGI_FORMAT_R32G32B32_FLOAT, 3, Float32 },
	{ DXGI_FORMAT_R32G32_FLOAT, 2, Float32 },
	{ DXGI_FORMAT_R32_FLOAT, 1, Float32 },

	{ DXGI_FORMAT_R32G32B32A32_SINT, 4, Int32 },
	{ DXGI_FORMAT_R32G32B32_SINT, 3, Int32 },
	{ DXGI_FORMAT_R32G32_SINT, 2, Int32 },
	{ DXGI_FORMAT_R32_SINT, 1, Int32 },

	{ DXGI_FORMAT_R32G32B32A32_UINT, 4, UInt32 },
	{ DXGI_FORMAT_R32G32B32_UINT, 3, UInt32 },
	{ DXGI_FORMAT_R32G32_UINT, 2, UInt32 },
	{ DXGI_FORMAT_R32_UINT, 1, UInt32 },

	{ DXGI_FORMAT_R32G32B32A32_TYPELESS, 4, Typeless32 },
	{ DXGI_FORMAT_R32G32B32_TYPELESS, 3, Typeless32 },
	{ DXGI_FORMAT_R32G32_TYPELESS, 2, Typeless32 },
	{ DXGI_FORMAT_R32_TYPELESS, 1, Typeless32 },

	// 16
	{ DXGI_FORMAT_R16G16B16A16_FLOAT, 4, Float16 },
	{ DXGI_FORMAT_R16G16_FLOAT, 2, Float16 },
	{ DXGI_FORMAT_R16_FLOAT, 1, Float16 },

	{ DXGI_FORMAT_R16G16B16A16_SINT, 4, Int16 },
	{ DXGI_FORMAT_R16G16_SINT, 2, Int16 },
	{ DXGI_FORMAT_R16_SINT, 1, Int16 },

	{ DXGI_FORMAT_R16G16B16A16_UINT, 4, UInt16 },
	{ DXGI_FORMAT_R16G16_UINT, 2, UInt16 },
	{ DXGI_FORMAT_R16_UINT, 1, UInt16 },

	{ DXGI_FORMAT_R16G16B16A16_SNORM, 4, Norm16 },
	{ DXGI_FORMAT_R16G16_SNORM, 2, Norm16 },
	{ DXGI_FORMAT_R16_SNORM, 1, Norm16 },

	{ DXGI_FORMAT_R16G16B16A16_UNORM, 4, UNorm16 },
	{ DXGI_FORMAT_R16G16_UNORM, 2, UNorm16 },
	{ DXGI_FORMAT_R16_UNORM, 1, UNorm16 },

	{ DXGI_FORMAT_R16G16B16A16_TYPELESS, 4, Typeless16 },
	{ DXGI_FORMAT_R16G16_TYPELESS, 2, Typeless16 },
	{ DXGI_FORMAT_R16_TYPELESS, 1, Typeless16 },

	// 8
	{ DXGI_FORMAT_R8G8B8A8_SINT, 4, Int8 },
	{ DXGI_FORMAT_R8G8_SINT, 2, Int8 },
	{ DXGI_FORMAT_R8_SINT, 1, Int8 },

	{ DXGI_FORMAT_R8G8B8A8_UINT, 4, UInt8 },
	{ DXGI_FORMAT_R8G8_UINT, 2, UInt8 },
	{ DXGI_FORMAT_R8_UINT, 1, UInt8 },

	{ DXGI_FORMAT_R8G8B8A8_SNORM, 4, Norm8 },
	{ DXGI_FORMAT_R8G8_SNORM, 2, Norm8 },
	{ DXGI_FORMAT_R8_SNORM, 1, Norm8 },

	{ DXGI_FORMAT_R8G8B8A8_UNORM, 4, UNorm8 },
	{ DXGI_FORMAT_R8G8_UNORM, 2, UNorm8 },
	{ DXGI_FORMAT_R8_UNORM, 1, UNorm8 },

	{ DXGI_FORMAT_R8G8B8A8_TYPELESS, 4, Typeless8 },
	{ DXGI_FORMAT_R8G8_TYPELESS, 2, Typeless8 },
	{ DXGI_FORMAT_R8_TYPELESS, 1, Typeless8 }
};

DXGI_FORMAT GetDXGI_Format(uint fields, StorageType storageType)
{
	for(uint i = 0; i < _countof(DXGI_Lookup); ++i)
	{
		DXGI_Lookup_t &d = DXGI_Lookup[i];
		if(d.fields == fields && d.storageType == storageType)
		{
			return d.format;
		}
	}
	return DXGI_FORMAT_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////

HRESULT Shader::CreateInputLayout()
{
	int n = mShaderDesc.InputParameters;
	mInputElements.resize(n);
	int vertexSize = 0;
	for(int i = 0; i < n; ++i)
	{
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
		if(strchr(d.SemanticName, '_') != null)
		{
			TRACE("Semantic name: %s\n", d.SemanticName);
			vector<string> tokens;
			tokenize(d.SemanticName, tokens, "_");
			if(tokens.size() > 1)
			{
				string &type_annotation = ToUpper(string(tokens[0]));
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
				if(fieldCount != 0)
				{
					d.Format = GetDXGI_Format(fieldCount, storageType);
				}
			}
		}
		if(d.Format == DXGI_FORMAT_UNKNOWN)
		{
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

