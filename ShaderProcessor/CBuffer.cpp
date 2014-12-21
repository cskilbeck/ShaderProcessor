//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

static char const *classNames[] =
{
	"scalar",
	"vector",
	"matrix_rows",
	"matrix_columns",
	"object",
	"struct",
	"interface_class",
	"interface_pointer"
};

//////////////////////////////////////////////////////////////////////

static bool isMatrix[] =
{
	false,	//"scalar",
	false,	//"vector",
	true,	//"matrix_rows",
	true,	//"matrix_columns",
	false,	//"object",
	false,	//"struct",
	false,	//"interface_class",
	false	//"interface_pointer"
};

// typeR[xC]

//////////////////////////////////////////////////////////////////////

static char const *typeNames[] =
{
	"void",
	"Bool",
	"Int",
	"Float",
	"String",
	"Texture",
	"Texture1d",
	"Texture2d",
	"Texture3d",
	"TextureCube",
	"Sampler",
	"PixelShader",
	"VertexShader",
	"UInt",
	"UInt8",
	"GeometryShader",
	"Rasterizer",
	"DepthStencil",
	"Blend",
	"Buffer",
	"CBuffer",
	"TBuffer",
	"Texture1dArray",
	"Texture2dArray",
	"Rendertargetview",
	"Depthstencilview",
	"Texture2dms",
	"Texture2dmsArray",
	"TextureCubeArray",
	"HullShader",
	"DomainShader",
	"interface_pointer",
	"ComputeShader",
	"Double",
	"rwtexture1d",
	"rwtexture1darray",
	"rwtexture2d",
	"rwtexture2darray",
	"rwtexture3d",
	"rwbuffer",
	"byteaddress_buffer",
	"rwbyteaddress_buffer",
	"structured_buffer",
	"rwstructured_buffer",
	"append_structured_buffer",
	"consume_structured_buffer"
};

//////////////////////////////////////////////////////////////////////

ConstantBuffer::ConstantBuffer(Shader *s, D3D11_SHADER_INPUT_BIND_DESC desc, ID3D11ShaderReflectionConstantBuffer *b)
	: Reportable(s, desc)
	, Name(null)
	, TotalSizeInBytes(0)
	, mReflectionCB(b)
{
	// get the details
	b->GetDesc(&mDesc);

	// fill them in
	Name = mDesc.Name;
	mParameters.clear();
	mParameterIDs.clear();
	TotalSizeInBytes = mDesc.Size;

	uint varCount = mDesc.Variables;

	for(uint j = 0; j < varCount; ++j)
	{
		ID3D11ShaderReflectionVariable *var = b->GetVariableByIndex(j);
		ID3D11ShaderReflectionType *type = var->GetType();
		Parameter *cbVar = new ConstantBuffer::Parameter();
		D3D11_SHADER_VARIABLE_DESC &v = cbVar->Variable;
		D3D11_SHADER_TYPE_DESC &t = cbVar->Type;
		DXV(var->GetDesc(&v));
		DXV(type->GetDesc(&t));
		mParameters.push_back(cbVar);
	}

	// get the details of the parameters
	for(uint j = 0; j < varCount; ++j)
	{
		Parameter *cbVar = mParameters[j];
		D3D11_SHADER_VARIABLE_DESC &v = cbVar->Variable;
		D3D11_SHADER_TYPE_DESC &t = cbVar->Type;

		uint end = (j < varCount - 1) ? mParameters[j + 1]->Variable.StartOffset : mDesc.Size;

		cbVar->padding = end - (v.StartOffset + v.Size);

		mParameterIDs[v.Name] = j;

		// copy the default value, if there is one, into the Defaults buffer
		uint offset = v.StartOffset;
		if(v.DefaultValue != null)
		{
			if(Defaults == null)
			{
				Defaults.reset(new byte[mDesc.Size]);
				memset(Defaults.get(), 0, mDesc.Size);
			}
			memcpy(Defaults.get() + offset, v.DefaultValue, v.Size);
			float *f = (float *)(Defaults.get() + offset);
		}
	}
}

//////////////////////////////////////////////////////////////////////

ConstantBuffer::~ConstantBuffer()
{
}

//////////////////////////////////////////////////////////////////////

void ConstantBuffer::StaticsOutput()
{
	printf("\t// %s Offsets\n", mDesc.Name);
	printf("\tDECLSPEC_SELECTANY extern CBufferOffset const %s_%s_Offsets[%d] = \n", mShader->Name().c_str(), mDesc.Name, mDesc.Variables);
	printf("\t{\n");
	auto i = mParameters.begin();
	while(true)
	{
		Parameter *p = (*i);
		D3D11_SHADER_VARIABLE_DESC &v = p->Variable;
		D3D11_SHADER_TYPE_DESC &t = p->Type;
		//string typeName = Format("%s%s%d", typeNames[t.Type], isMatrix[t.Class] ? Format("%dx", t.Rows).c_str() : "", t.Columns);
		printf("\t\t{ \"%s\", %d }", v.Name, v.StartOffset);

		if(++i == mParameters.end())
		{
			printf("\n");
			break;
		}
		else
		{
			printf(",\n");
		}
	}
	printf("\t};\n\n");

	if(Defaults != null)
	{
		printf("\t// %s Defaults\n", mDesc.Name);
		printf("\tDECLSPEC_SELECTANY extern uint32 const %s_%s_Defaults[%d] = \n\t{\n", mShader->Name().c_str(), mDesc.Name, mDesc.Size / sizeof(uint32));
		uint varCount = (uint)mParameters.size();
		for(uint i = 0; i < varCount; ++i)
		{
			Parameter *p = mParameters[i];
			D3D11_SHADER_VARIABLE_DESC &v = p->Variable;
			printf("\t\t");
			uint32 *data = (uint32 *)(Defaults.get() + v.StartOffset);
			bool lastOne = i == (varCount - 1);
			uint end = (lastOne) ? mDesc.Size : mParameters[i + 1]->Variable.StartOffset;
			uint pad = (end - (v.StartOffset + v.Size));
			uint slots = (v.Size + pad) / sizeof(uint32);

			for(uint j = 0; j < slots; ++j)
			{
				printf("0x%08x%s", *data++, (j < slots - 1) ? "," : Format("%s // %s", lastOne ? " " : ",", v.Name).c_str());
			}
			printf("\n");
		}
		printf("\t};\n\n");
	}
}

//////////////////////////////////////////////////////////////////////

void ConstantBuffer::MemberOutput()
{
	// need to track the size of the struct and insert padding so no member crosses a 1x4 register boundary...

	uint padID = 0;
	printf("\t\tstruct %s_t\n\t\t{\n", mDesc.Name);
	for(auto i = mParameters.begin(); i != mParameters.end(); ++i)
	{
		Parameter *p = (*i);
		D3D11_SHADER_VARIABLE_DESC &v = p->Variable;
		D3D11_SHADER_TYPE_DESC &t = p->Type;
		string typeName = Format("%s%s%d", typeNames[t.Type], isMatrix[t.Class] ? Format("%dx", t.Rows).c_str() : "", t.Columns);
		printf("\t\t\t%s %s;", typeName.c_str(), v.Name);
		if(p->padding != 0)
		{
			printf("\t\t\tbyte pad%d[%d];", padID++, p->padding);
		}
		printf("\n");
	}

	//ConstantBuffer<GridStuff_t, 2, SimplePixelShader_GridStuff_Offsets> GridStuff;
	string s = Format("ConstantBuffer<%s_t, %d, %s_%s_Offsets> %s", mDesc.Name, mDesc.Variables, mShader->Name().c_str(), mDesc.Name, mDesc.Name);
	printf("\t\t};\n\t\t%s;\n", s.c_str());
}

//////////////////////////////////////////////////////////////////////

void ConstantBuffer::ConstructorOutput()
{
	printf("%s(%s)\n", mDesc.Name, Defaults == null ? "" : Format("%s_%s_Defaults", mShader->Name().c_str(), mDesc.Name).c_str());
}

//////////////////////////////////////////////////////////////////////

