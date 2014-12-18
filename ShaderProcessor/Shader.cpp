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

static ISMap shader_input_type_names =
{
	{ D3D_SIT_CBUFFER, "ConstantBuffer" },
	{ D3D_SIT_TBUFFER, "TextureBuffer" },
	{ D3D_SIT_TEXTURE, "Texture" },
	{ D3D_SIT_SAMPLER, "SamplerState" },
	{ D3D_SIT_UAV_RWTYPED, "UnorderedAccessViewRWTyped" },
	{ D3D_SIT_STRUCTURED, "StructuredInput" },
	{ D3D_SIT_UAV_RWSTRUCTURED, "UnorderedAccessViewRWStructured" },
	{ D3D_SIT_BYTEADDRESS, "ByteAddress" },
	{ D3D_SIT_UAV_RWBYTEADDRESS, "RWByteAddress" },
	{ D3D_SIT_UAV_APPEND_STRUCTURED, "AppendStructured" },
	{ D3D_SIT_UAV_CONSUME_STRUCTURED, "ConsumeStructured" },
	{ D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER, "UnorderedAccessViewRWStructuredWithCounter" }
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
	//	Print("\t%s%s *%s;\n", GetFrom(shader_input_type_names, desc.Type).c_str(), GetFrom(shader_input_dimension_names, desc.Dimension).c_str(), desc.Name);
	Texture2D *tb = new Texture2D(this, desc);
	mTextures.emplace(mTextures.begin() + desc.BindPoint, tb);
	mTextureIDs[string(desc.Name)] = desc.BindPoint;
	return tb;
}

//////////////////////////////////////////////////////////////////////

Reportable *Shader::CreateSamplerBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	//	Print("\t%s%s *%s;\n", GetFrom(shader_input_type_names, desc.Type).c_str(), GetFrom(shader_input_dimension_names, desc.Dimension).c_str(), desc.Name);
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
	uint i = desc.BindPoint;
	mTextureBufferIDs[string(desc.Name)] = i;
	mTextureBuffers.emplace(mTextureBuffers.begin() + i, tp);
	//	Print("\t%s %s", GetFrom(shader_input_type_names, desc.Type).c_str(), desc.Name);
	//	Print("\n\t{\n");
	//	Print("\t}\n");
	return tp;
}

//////////////////////////////////////////////////////////////////////

Reportable *Shader::CreateBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	TRACE("Binding at %d is a %s called %s\n", desc.BindPoint, GetFrom(shader_input_type_names, desc.Type).c_str(), desc.Name);

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

	TRACE("ConstantBuffers: %d\nBindingSlots: %d\n", numConstantBuffers, numBindingSlots);

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
//		Print("\n");
	}
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
	Print("%s", header.c_str());
	for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	{
		(*i)->StaticsOutput();
	}
	Print("\tstruct %s_t", Name().c_str());	// TODO: add : PixelShader or : VertexShader or whatever...
	Print("\n\t{\n");
	for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	{
		(*i)->MemberOutput();
		Print("\n");
	}
	Print(constructor.c_str(), Name().c_str());
	char const *pre = "\t\t\t: ";
	for(auto i = mBindings.begin(); i != mBindings.end(); ++i)
	{
		Print(pre);
		(*i)->ConstructorOutput();
		pre = "\t\t\t, ";
	}
	Print("\t\t{\n\t\t}\n");
	Print("\t};\n\n\tDECLSPEC_SELECTANY %s_t %s;\n", Name().c_str(), Name().c_str());
	Print("%s", footer.c_str());
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

