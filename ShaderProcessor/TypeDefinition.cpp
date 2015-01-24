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
	"Void",
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

TypeDefinition::TypeDefinition(ID3D11ShaderReflection *reflection, uint index)
{
	// get the desc
	ID3D11ShaderReflectionConstantBuffer *b = reflection->GetConstantBufferByIndex(index);
	b->GetDesc(&mDesc);

	// get some bits
	Name = mDesc.Name;
	TotalSizeInBytes = mDesc.Size;
	FieldCount = mDesc.Variables;

	// 1st pass, get the descs
	for(uint j = 0; j < FieldCount; ++j)
	{
		ID3D11ShaderReflectionVariable *var = b->GetVariableByIndex(j);
		ID3D11ShaderReflectionType *type = var->GetType();
		Field *cbVar = new Field();
		D3D11_SHADER_VARIABLE_DESC &v = cbVar->Variable;
		D3D11_SHADER_TYPE_DESC &t = cbVar->Type;
		DXV(var->GetDesc(&v));
		DXV(type->GetDesc(&t));
		mFields.push_back(cbVar);
		mFieldIDs[v.Name] = j;
	}

	// 2nd pass, work out padding and get defaults
	for(uint j = 0; j < FieldCount; ++j)
	{
		Field *cbVar = mFields[j];
		D3D11_SHADER_VARIABLE_DESC &v = cbVar->Variable;
		D3D11_SHADER_TYPE_DESC &t = cbVar->Type;

		// get where the next field starts (or the end of the struct if this is the last one)
		uint nextStart = (j < FieldCount - 1) ? mFields[j + 1]->Variable.StartOffset : mDesc.Size;

		// padding is difference between the end of this one and start of next		
		cbVar->padding = nextStart - (v.StartOffset + v.Size);

		// copy the default value, if there is one, into the Defaults mDesc
		if(v.DefaultValue != null)
		{
			if(Defaults == null)
			{
				Defaults.reset(new byte[mDesc.Size]);
				memset(Defaults.get(), 0, mDesc.Size);
			}
			memcpy(Defaults.get() + v.StartOffset, v.DefaultValue, v.Size);
		}
	}
}

//////////////////////////////////////////////////////////////////////

TypeDefinition::~TypeDefinition()
{
}

//////////////////////////////////////////////////////////////////////

static void OutputTable(uint32 *table, uint count, char const *name)
{
	TRACE("uint32 half::%s[%d] = {", name, count);
	char const *sep = "";
	string sname = "// Name";
	for(uint i = 0; i < count; ++i)
	{
		TRACE(sep);
		if((i % 8) == 0)
		{
			TRACE("%s\n\t", sname.c_str());
			name = "";
		}
		TRACE("0x%08x", table[i]);
		sep = ",";
	}
	TRACE("\n};\n");
}

void TypeDefinition::StaticsOutput(string const &shaderName)
{
	printf("\t\t// %s Offsets\n", mDesc.Name);
	printf("\t\tDECLSPEC_SELECTANY extern CBufferOffset const %s_Offsets[%d] = \n", mDesc.Name, mDesc.Variables);
	printf("\t\t{");
	char const *sep = "";
	for(uint i = 0; i < FieldCount; ++i)
	{
		D3D11_SHADER_VARIABLE_DESC &v = mFields[i]->Variable;
		printf("%s\n\t\t\t{ \"%s\", %d }", sep, v.Name, v.StartOffset);
		sep = ",";
	}
	printf("\n\t\t};\n\n");

	if(Defaults != null)
	{
		printf("\t\t// %s Defaults\n", mDesc.Name);
		printf("\t\tDECLSPEC_SELECTANY extern uint32 const %s_Defaults[%d] = \n\t\t{", mDesc.Name, mDesc.Size / sizeof(uint32));
		sep = "";
		for(uint i = 0; i < FieldCount; ++i)
		{
			D3D11_SHADER_VARIABLE_DESC &v = mFields[i]->Variable;
			uint32 *data = (uint32 *)(Defaults.get() + v.StartOffset);
			bool lastOne = i == (FieldCount - 1);
			uint end = (lastOne) ? mDesc.Size : mFields[i + 1]->Variable.StartOffset;
			uint pad = (end - (v.StartOffset + v.Size));
			uint slots = (v.Size + pad) / sizeof(uint32);
			string eol = Format("// %s", v.Name);
			char const *sol = "\n\t\t\t";
			char const *sname = v.Name;
			for(uint j = 0; j < slots; ++j)
			{
				printf(sep);
				if((j % 4) == 0)
				{
					printf("%s%s\n\t\t\t", sol, eol.c_str());
					sol = "";
					eol = "";
				}
				printf("0x%08x", *data++);
				sep = ",";
			}
		}
		printf("\n\t\t};\n");
	}
}

//////////////////////////////////////////////////////////////////////

void TypeDefinition::MemberOutput(string const &shaderName)
{
	// need to track the size of the struct and insert padding so no member crosses a 1x4 register boundary...

	uint padID = 0;
	printf("\t\tstruct %s_t\n\t\t{\n", mDesc.Name);
	for(auto i = mFields.begin(); i != mFields.end(); ++i)
	{
		Field *p = (*i);
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
	printf("\t\t};\n\n");
	printf("\t\t%s_t %s;\n\n", mDesc.Name, mDesc.Name);

	//ConstantBuffer<GridStuff_t, 2, SimplePixelShader_GridStuff_Offsets> GridStuff;
	//string s = Format("ConstantBuffer<%s_t, %d, %s_%s_Offsets> %s", mDesc.Name, mDesc.Variables, shaderName.c_str(), mDesc.Name, mDesc.Name);
	//printf("\t\t};\n\t\t%s;\n", s.c_str());
}
