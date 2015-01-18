//////////////////////////////////////////////////////////////////////
// Make it so it can compile multiple shader types in one run
// Sort out the semantic name annotation parser & emit InputLayoutDefinitions
// Make it a proper Compile type in VS2013

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

using ISMap = std::map < int, string >;

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

//////////////////////////////////////////////////////////////////////

Binding *Shader::CreateBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	//TRACE("Binding %s at %d is a %s called %s\n", GetFrom(shader_input_type_names, desc.Type).c_str(), desc.BindPoint, GetFrom(shader_input_type_names, desc.Type).c_str(), desc.Name);
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
static string constructor = "\t\t// Constructor\n\t\t%s_t()\n";

//////////////////////////////////////////////////////////////////////

void Shader::Output()
{
	unsigned __int32 bob = 1;
	bob = _rotr(bob, 10);
	printf("%s", header.c_str());

	printf("\tstruct %s_t", Name().c_str());	// TODO: add : PixelShader or : VertexShader or whatever...
	printf("\n\t{\n");
	for(auto i = mDefinitions.begin(); i != mDefinitions.end(); ++i)
	{
		(*i)->MemberOutput(this->Name());
	}
	printf("\t};\n");

	//for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	//{
	//	(*i)->StaticsOutput();
	//}
	//for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	//{
	//	(*i)->MemberOutput();
	//	printf("\n");
	//}
	//printf(constructor.c_str(), Name().c_str());
	//char const *pre = "\t\t\t: ";
	//for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	//{
	//	printf(pre);
	//	(*i)->ConstructorOutput();
	//	pre = "\t\t\t, ";
	//}
	//printf("\t\t{\n\t\t}\n");
	//printf("\t};\n\n\tDECLSPEC_SELECTANY %s_t %s;\n", Name().c_str(), Name().c_str());
	printf("%s", footer.c_str());
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
// Missing some wacky short float types...

enum StorageType
{
	Float,
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
	UNorm8
};

uint StorageSize[] =
{
	32,		// Float,
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
};

struct InputType
{
	char const *name;
	int fieldCount;	// if this is 0, infer from input type
	StorageType storageType;
};

InputType type_suffix[] =
{
	"float", 0, Float,
	"int32", 0, Int32,
	"int16", 0, Int16,
	"int8", 0, Int8,
	"uint32", 0, UInt32,
	"uint16", 0, UInt16,
	"uint8", 0, UInt8,
	"norm32", 0, Norm32,
	"norm16", 0, Norm16,
	"norm8", 0, Norm8,
	"unorm32", 0, UNorm32,
	"unorm16", 0, UNorm16,
	"unorm8", 0, UNorm8
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

//////////////////////////////////////////////////////////////////////

HRESULT Shader::CreateInputLayout()
{
	int n = mShaderDesc.InputParameters;
	vector<D3D11_INPUT_ELEMENT_DESC> ied(n);
	int vertexSize = 0;
	for(int i = 0; i < n; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC desc;
		DX(mReflector->GetInputParameterDesc(i, &desc));
		D3D11_INPUT_ELEMENT_DESC &d = ied[i];
		d.SemanticName = desc.SemanticName;
		d.SemanticIndex = desc.SemanticIndex;
		d.InputSlot = 0;
		d.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		d.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		d.InstanceDataStepRate = 0;
		d.Format = DXGI_FORMAT_UNKNOWN;

		// if they ask, try to find the right storage format
		if(strchr(d.SemanticName, '_') != null)
		{
			vector<string> tokens;
			tokenize(string(d.SemanticName), tokens, string("_"));
			if(tokens.size() > 1)
			{
				int fieldCount;
				string &type_annotation = tokens[1];
				for(int i = 0; i < _countof(type_suffix); ++i)
				{
					if(type_annotation.compare(type_suffix[i].name) == 0)
					{
						int fc = type_suffix[i].fieldCount;
						fieldCount = (fc == 0) ? CountBits(desc.Mask) : fc;
					}
				}
			}
		}
		if(d.Format == DXGI_FORMAT_UNKNOWN)
		{
			d.Format = formats[CountBits(desc.Mask) - 1][desc.ComponentType];
		}

		// if there's no underscore in the semantic name
		//    if there's a match in the list of InputTypes
		//       if fieldCount == 0
		//           Count = input field size
		//       else
		//           Count = InputType.fieldCount
		//           if Count != input field size
		//              Warn of field count mismatch
		//    else
		//       Warn of unknown InputType
		//       Type = input type
		// else
		//    Type = input type
		// Now we know the type of variable, the Type for storage and the field count
		// Look it up

		uint size1 = SizeOfFormatElement(d.Format);
		uint size2 = size1 / 8;
		vertexSize += size2;
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

