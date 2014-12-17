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

Shader::Shader(tstring const &filename)
	: mStartIndex(0)
	, mName(filename)
{
	Print("struct %s : Shader\n{\n", StringFromWideString(mName).c_str());
}

//////////////////////////////////////////////////////////////////////

Shader::~Shader()
{
	Print("};\n");
	Destroy();
}

//////////////////////////////////////////////////////////////////////

template <typename T> static void AddAt(vector<T> &v, uint index, T const &item)
{
	if(index >= (uint)v.size())
	{
		v.resize(index + 1);
	}
	v[index] = item;
}

//////////////////////////////////////////////////////////////////////

void Shader::DeleteConstantBuffers()
{
	mConstBufferIDs.clear();
	for(auto i = mConstantBuffers.begin(); i != mConstantBuffers.end(); ++i)
	{
		delete (*i);
	}
	mConstantBuffers.clear();
	mBuffers.clear();
}

//////////////////////////////////////////////////////////////////////

HRESULT Shader::CreateConstantBuffer(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	string typeName = GetFrom(shader_input_type_names, desc.Type);
	ConstantBuffer *cb = new ConstantBuffer();
	uint i = desc.BindPoint;
	DX(cb->Create(mReflector->GetConstantBufferByIndex(i)));
	mConstBufferIDs[string(cb->Name)] = i;
	AddAt(mConstantBuffers, i, cb);
	AddAt(mBuffers, i, cb->mConstantBuffer);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

void Shader::Output()
{
	string header = R"(
//////////////////////////////////////////////////////////////////////

namespace HLSL
{
#pragma pack(push, 4)

)";

	string footer = R"(
#pragma pack(pop)

} // HLSL
)";

	string constructor = R"(		// Constructor
		$s_t()
)";

	// output header
	Print("%s", header.c_str());

	// call StaticsOutput for each ConstantBuffer

	Print("\tstruct %s_t", mName.c_str());	// TODO: add : PixelShader or : VertexShader or whatever...
	Print("\n\t{\n");

	// call MemberOutput for each ConstantBuffer, Texture and Sampler

	// output constructor preamble
	Print(constructor.c_str(), mName.c_str());

	// call ConstructorOutput for each ConstantBuffer, Texture and Sampler, use : for 1st one and , for subsequent
	char const *pre = ":";
	
	

	Print("\t};\n\tstruct %s_t %s;\n", mName.c_str(), mName.c_str());

	// output footer
	Print("%s", footer.c_str());
}

//////////////////////////////////////////////////////////////////////
// No type information in TextureBuffer available from D3DReflection!?

HRESULT Shader::CreateTextureBuffer(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	TextureBuffer *tp = new TextureBuffer();
	uint i = desc.BindPoint;
	mTextureBufferIDs[string(desc.Name)] = i;
	AddAt(mTextureBuffers, i, tp);
	Print("\t%s %s", GetFrom(shader_input_type_names, desc.Type).c_str(), desc.Name);
	Print("\n\t{\n");
	Print("\t}\n");
	return S_OK;
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

//////////////////////////////////////////////////////////////////////

HRESULT Shader::CreateBindings()
{
	uint numBindingSlots = mShaderDesc.BoundResources;

	for(uint i = 0; i < numBindingSlots; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC d;
		mReflector->GetResourceBindingDesc(i, &d);
		DX(CreateBinding(d));
		Print("\n");
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

HRESULT Shader::CreateTextureBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	Print("\t%s%s *%s;\n", GetFrom(shader_input_type_names, desc.Type).c_str(), GetFrom(shader_input_dimension_names, desc.Dimension).c_str(), desc.Name);
	AddAt(mTextures, desc.BindPoint, (ID3D11ShaderResourceView *)null);
	mTextureIDs[string(desc.Name)] = desc.BindPoint;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

HRESULT Shader::CreateSamplerBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	Print("\t%s%s *%s;\n", GetFrom(shader_input_type_names, desc.Type).c_str(), GetFrom(shader_input_dimension_names, desc.Dimension).c_str(), desc.Name);
	AddAt(mSamplers, desc.BindPoint, (ID3D11SamplerState *)null);
	mSamplerIDs[desc.Name] = desc.BindPoint;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

HRESULT Shader::CreateBinding(D3D11_SHADER_INPUT_BIND_DESC desc)
{
	switch(desc.Type)
	{
		case D3D_SHADER_INPUT_TYPE::D3D10_SIT_TEXTURE: return CreateTextureBinding(desc);
		case D3D_SHADER_INPUT_TYPE::D3D10_SIT_SAMPLER: return CreateSamplerBinding(desc);
		case D3D_SHADER_INPUT_TYPE::D3D10_SIT_CBUFFER: return CreateConstantBuffer(desc);
		case D3D_SHADER_INPUT_TYPE::D3D10_SIT_TBUFFER: return CreateTextureBuffer(desc);
		default: return ERROR_BAD_ARGUMENTS;
	}
}
