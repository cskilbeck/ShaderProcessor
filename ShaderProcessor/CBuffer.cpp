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

	// get the details of the parameters
	for(uint j = 0; j < mDesc.Variables; ++j)
	{
		ID3D11ShaderReflectionVariable *var = b->GetVariableByIndex(j);
		ID3D11ShaderReflectionType *type = var->GetType();
		ConstantBuffer::Parameter *cbVar = new ConstantBuffer::Parameter();
		D3D11_SHADER_VARIABLE_DESC &v = cbVar->Variable;
		D3D11_SHADER_TYPE_DESC &t = cbVar->Type;
		DXV(var->GetDesc(&v));
		DXV(type->GetDesc(&t));
		mParameters.push_back(cbVar);
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
		Print("\t\t{ \"%s\", %d }", v.Name, v.StartOffset);
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
		i = mParameters.begin();
		while(true)
		{
			Parameter *p = *i++;
			D3D11_SHADER_VARIABLE_DESC &v = p->Variable;
			D3D11_SHADER_TYPE_DESC &t = p->Type;
			printf("\t\t");
			uint32 *data = (uint32 *)(Defaults.get() + v.StartOffset);

			uint slots = v.Size;
			uint endOfThisOne = v.StartOffset + v.Size;
			uint startOfNextOne = endOfThisOne;

			if(i != mParameters.end())
			{
				startOfNextOne = (*i)->Variable.StartOffset;
			}
			else if(endOfThisOne < mDesc.Size)
			{
				startOfNextOne = mDesc.Size;
			}
			uint padding = (startOfNextOne - endOfThisOne);

			slots += padding;

			slots /= sizeof(uint32);

			for(uint j = 0; j < slots; ++j)
			{
				printf("0x%08x%s", *data++, (j < slots - 1) ? "," : Format("%s // %s", (i != mParameters.end()) ? "," : " ", v.Name).c_str());
			}
			printf("\n");

			if(i == mParameters.end())
			{
				break;
			}
		}
		printf("\t};\n\n");
	}
}

//////////////////////////////////////////////////////////////////////

void ConstantBuffer::MemberOutput()
{
	Print("\t\tstruct %s_t\n\t\t{\n", mDesc.Name);
	for(auto i = mParameters.begin(); i != mParameters.end(); ++i)
	{
		Parameter *p = (*i);
		D3D11_SHADER_VARIABLE_DESC &v = p->Variable;
		D3D11_SHADER_TYPE_DESC &t = p->Type;
		string typeName = Format("%s%s%d", typeNames[t.Type], isMatrix[t.Class] ? Format("%dx", t.Rows).c_str() : "", t.Columns);
		Print("\t\t\t%s %s;\n", typeName.c_str(), v.Name);
	}

	//ConstantBuffer<GridStuff_t, 2, SimplePixelShader_GridStuff_Offsets> GridStuff;
	string s = Format("ConstantBuffer<%s_t, %d, %s_%s_Offsets> %s", mDesc.Name, mDesc.Variables, mShader->Name().c_str(), mDesc.Name, mDesc.Name);
	Print("\t\t};\n\t\t%s;\n", s.c_str());
}

//////////////////////////////////////////////////////////////////////

void ConstantBuffer::ConstructorOutput()
{
	Print("%s(%s)\n", mDesc.Name, Defaults == null ? "" : Format("%s_%s_Defaults", mShader->Name().c_str(), mDesc.Name).c_str());
}

//////////////////////////////////////////////////////////////////////

