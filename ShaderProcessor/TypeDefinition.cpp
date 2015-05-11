//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

using namespace Printer;

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

uint Variable::padID;

char const *VariableType::typeNames[] =
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
	: mIndex(index)
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
		mVariables.push_back(new Variable(reflection, b->GetVariableByIndex(j)));
	}

	// 2nd pass, work out padding and get defaults
	for(uint j = 0; j < mVariables.size(); ++j)
	{
		Variable *var = mVariables[j];

		// get where the next field starts (or the end of the struct if this is the last one)
		uint nextStart = (j < FieldCount - 1) ? mVariables[j + 1]->StartOffset() : mDesc.Size;

		// padding is difference between the end of this one and start of next		
		var->padding = nextStart - var->EndOffset();

		// copy the default value, if there is one, into the Defaults mDesc
		if(var->Defaults() != null)
		{
			memcpy(DefaultsBuffer() + var->StartOffset(), var->Defaults(), var->Size());
		}
	}
}

//////////////////////////////////////////////////////////////////////

TypeDefinition::~TypeDefinition()
{
}

//////////////////////////////////////////////////////////////////////

void TypeDefinition::StaticsOutput(string const &shaderName)
{
	OutputCommentLine("%s offsets", mDesc.Name);
	OutputLine("extern ConstBufferOffset const WEAKSYM %s_%s_Offsets[%d] =", shaderName.c_str(), mDesc.Name, mDesc.Variables);
	OutputIndent("{");
	Indent();
	char const *sep = "";
	for(uint i = 0; i < mVariables.size(); ++i)
	{
		Variable &v = *mVariables[i];
		Output(sep);
		OutputLine();
		OutputIndent();
		Output("{ \"%s\", %d, %d, %d }", v.Name(), v.StartOffset(), v.Type->Type(), v.Type->Elements());
		sep = ",";
	}
	OutputLine();
	UnIndent("};");
	OutputLine();

	if(mDefaultBuffer == null)
	{
		OutputComment("no defaults for %s", mDesc.Name);
	}
	else
	{
		OutputLine("// %s defaults\n", mDesc.Name);
		OutputLine("extern uint32 __declspec(align(16)) WEAKSYM %s_%s_Defaults[%d] =", shaderName.c_str(), mDesc.Name, mDesc.Size / sizeof(uint32));
		OutputIndent("{");
		Indent();
		sep = "";
		for(uint i = 0; i < FieldCount; ++i)
		{
			Variable &v = *mVariables[i];
			uint32 *data = (uint32 *)(v.Defaults() + v.StartOffset());
			bool lastOne = i == (FieldCount - 1);
			uint end = (lastOne) ? mDesc.Size : mVariables[i + 1]->StartOffset();
			uint pad = (uint)(end - (v.StartOffset() + v.Size()));
			uint slots = (uint)(v.Size() + pad) / sizeof(uint32);
			string eol = Format("// %s", v.Name());
			char const *sol = "\n";
			char const *sname = v.Name();
			for(uint j = 0; j < slots; ++j)
			{
				Output(sep);
				if((j % 4) == 0)
				{
					Output(sol);
					OutputIndent();
					Output(eol.c_str());
					OutputLine();
					OutputIndent();
					sol = "";
					eol = "";
				}
				Output("0x%08x", *data++);
				sep = ",";
			}
		}
		OutputLine();
		UnIndent("};");
		OutputLine();
	}
}

//////////////////////////////////////////////////////////////////////

void TypeDefinition::MemberOutput(string const &shaderName, uint bindPoint)
{
	Variable::ResetPaddingID();

	OutputLine("struct %s_t : Aligned16", mDesc.Name);
	OutputLine("{");
	Indent();
	for(auto p : mVariables)
	{
		p->OutputDefinition();
		OutputLine();
	}
	OutputLine("enum { Index = %d };", mIndex);
	UnIndent("};");
	OutputLine();
	OutputLine("ConstBuffer<%s_t> *%s;", mDesc.Name, mDesc.Name);
	OutputLine("enum { %s_index = %d };", mDesc.Name, mIndex);
	OutputLine();
}

//////////////////////////////////////////////////////////////////////

void TypeDefinition::ConstructorOutput(int bindPoint)
{
	//OutputLine(", %s()", mDesc.Name);
}

//////////////////////////////////////////////////////////////////////

void TypeDefinition::CreateOutput(int bindPoint)
{
	string defaultStr = "null";
	if(mDefaultBuffer != null)
	{
		defaultStr = Format("%s_%s_Defaults", Printer::ShaderName().c_str(), mDesc.Name);
	}
//	OutputLine("%s.Create(%u, %s_%s_Offsets, %s, this, %d, %d);", mDesc.Name, mVariables.size(), Printer::ShaderName().c_str(), mDesc.Name, defaultStr.c_str(), mIndex, bindPoint);
	OutputLine("DXR(CreateConstBuffer<%s_t>(\"%s\", &%s,%u, %s_%s_Offsets, %s, this, %d));", mDesc.Name, mDesc.Name, mDesc.Name, mVariables.size(), Printer::ShaderName().c_str(), mDesc.Name, defaultStr.c_str(), bindPoint);
}

void TypeDefinition::ReleaseOutput()
{
	OutputLine("%s->Release();", mDesc.Name);
}

void TypeDefinition::DefinitionOutput()
{
	// Output enough info to disambiguate
}