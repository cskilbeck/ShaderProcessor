//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

using ISMap = std::map < int, string >;

static string const &GetFrom(ISMap const &map, int x)
{
	static string const unknown("");
	auto i = map.find(x);
	return (i != map.end()) ? i->second : unknown;
}

static ISMap constant_buffer_type_names =
{
	{ D3D_CT_CBUFFER, "ConstantBuffer" },
	{ D3D_CT_TBUFFER, "TextureBuffer" },
	{ D3D_CT_INTERFACE_POINTERS, "InterfacePointers" },
	{ D3D_CT_RESOURCE_BIND_INFO, "ResourceBindingInfo" }
};

static ISMap shader_input_type_names =
{
	{ D3D_SIT_CBUFFER, "ConstantBuffer" },										// has 'Constant Buffer'
	{ D3D_SIT_TBUFFER, "TextureBuffer" },										// has 'Constant Buffer'
	{ D3D_SIT_TEXTURE, "Texture" },
	{ D3D_SIT_SAMPLER, "SamplerState" },
	{ D3D_SIT_UAV_RWTYPED, "RW_UAVTyped" },
	{ D3D_SIT_STRUCTURED, "StructuredInput" },									// has 'Constant Buffer'
	{ D3D_SIT_UAV_RWSTRUCTURED, "RW_UAVStructured" },
	{ D3D_SIT_BYTEADDRESS, "ByteAddress" },
	{ D3D_SIT_UAV_RWBYTEADDRESS, "RW_ByteAddress" },
	{ D3D_SIT_UAV_APPEND_STRUCTURED, "AppendStructured" },						// has 'Constant Buffer'
	{ D3D_SIT_UAV_CONSUME_STRUCTURED, "ConsumeStructured" },					// has 'Constant Buffer'
	{ D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER, "RW_AUVStructuredWithCounter" }	// has 'Constant Buffer'
};

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

void Shader::DeleteConstantBuffers()
{
	mConstBufferIDs.clear();
	for(auto i = mConstantBuffers.begin(); i != mConstantBuffers.end(); ++i)
	{
		delete (*i);
	}
	mConstantBuffers.clear();
}

//////////////////////////////////////////////////////////////////////

Reportable *Shader::CreateTextureBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	// Deal with Buffer dimensions here...
	//	printf("\t%s%s *%s;\n", GetFrom(shader_input_type_names, desc.Type).c_str(), GetFrom(shader_input_dimension_names, desc.Dimension).c_str(), desc.Name);
	Texture2D *tb = new Texture2D(this, desc);
	mTextures.resize(Max(mTextures.size() + 1, (size_t)desc.BindPoint));
	mTextures.emplace(mTextures.begin() + desc.BindPoint, tb);
	mTextureIDs[string(desc.Name)] = desc.BindPoint;
	return tb;
}

//////////////////////////////////////////////////////////////////////

Reportable *Shader::CreateSamplerBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	//	printf("\t%s%s *%s;\n", GetFrom(shader_input_type_names, desc.Type).c_str(), GetFrom(shader_input_dimension_names, desc.Dimension).c_str(), desc.Name);
	SamplerState *ss = new SamplerState(this, desc);
	mSamplers.emplace(mSamplers.begin() + desc.BindPoint, ss);
	mSamplerIDs[desc.Name] = desc.BindPoint;
	return ss;
}

//////////////////////////////////////////////////////////////////////

Reportable *Shader::CreateConstantBuffer(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	ConstantBuffer *cb = new ConstantBuffer(this, desc, mReflector->GetConstantBufferByIndex(desc.BindPoint));
	mConstBufferIDs[desc.Name] = desc.BindPoint;
	mConstantBuffers.emplace(mConstantBuffers.begin() + desc.BindPoint, cb);
	return cb;
}

//////////////////////////////////////////////////////////////////////

Reportable *Shader::CreateTextureBufferBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	TextureBuffer *tp = new TextureBuffer(this, desc);
	mTextureBufferIDs[desc.Name] = desc.BindPoint;
	mTextureBuffers.emplace(mTextureBuffers.begin() + desc.BindPoint, tp);
	return tp;
}

//////////////////////////////////////////////////////////////////////

Reportable *Shader::CreateBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	TRACE("Binding %s at %d is a %s called %s\n", GetFrom(shader_input_type_names, desc.Type).c_str(), desc.BindPoint, GetFrom(shader_input_type_names, desc.Type).c_str(), desc.Name);

	switch(desc.Type)
	{
		case D3D_SHADER_INPUT_TYPE::D3D10_SIT_TEXTURE: return CreateTextureBinding(desc);
		case D3D_SHADER_INPUT_TYPE::D3D10_SIT_SAMPLER: return CreateSamplerBinding(desc);
		case D3D_SHADER_INPUT_TYPE::D3D10_SIT_CBUFFER: return CreateConstantBuffer(desc);
		case D3D_SHADER_INPUT_TYPE::D3D10_SIT_TBUFFER: return CreateTextureBufferBinding(desc);
		default: return null;
	}
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
		Reportable *r = CreateBinding(d);
		if(r != null)
		{
			mBindings.push_back(r);
		}
//		printf("\n");
	}

	delete[] buffers;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////

Shader::Shader(tstring const &filename)
	: mStartIndex(0)
	, mName(filename)
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
	printf("%s", header.c_str());
	for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	{
		(*i)->StaticsOutput();
	}
	printf("\tstruct %s_t", Name().c_str());	// TODO: add : PixelShader or : VertexShader or whatever...
	printf("\n\t{\n");
	for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	{
		(*i)->MemberOutput();
		printf("\n");
	}
	printf(constructor.c_str(), Name().c_str());
	char const *pre = "\t\t\t: ";
	for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	{
		printf(pre);
		(*i)->ConstructorOutput();
		pre = "\t\t\t, ";
	}
	printf("\t\t{\n\t\t}\n");
	printf("\t};\n\n\tDECLSPEC_SELECTANY %s_t %s;\n", Name().c_str(), Name().c_str());
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

HRESULT Shader::Create(void const *blob, size_t size)
{
	DX(D3DReflect(blob, size, IID_ID3D11ShaderReflection, (void **)&mReflector));
	mReflector->GetDesc(&mShaderDesc);
	DX(CreateBindings());
	Output();
	return S_OK;

	// Shader types:
	//  Pixel
	//  Vertex
	//  Domain
	//  Geometry
	//  Hull
	//  Compute

	// So, each bind point corresponds to one of these:

	// Sampler
	// Buffer
	// ConstantBuffer

	// Binding types:
	//  ShaderResources
	//  ConstantBuffers
	//  Samplers
	//  Buffers [??]
	//  UnorderedAccessViews	(PS, CS only)

}

//////////////////////////////////////////////////////////////////////

HRESULT Shader::Destroy()
{
	mReflector.Release();
	DeleteConstantBuffers();
	mTextureIDs.clear();
	mSamplerIDs.clear();
	return S_OK;
}

