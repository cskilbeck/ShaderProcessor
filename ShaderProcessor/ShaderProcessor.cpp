//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

using namespace DirectX;

//////////////////////////////////////////////////////////////////////

using option::Option;
vector<Option> options;

//////////////////////////////////////////////////////////////////////

struct ShaderArgType
{
	int arg;
	ShaderType type;
};

//////////////////////////////////////////////////////////////////////

ShaderArgType shaderTypes[] =
{
	{ VERTEX_MAIN, ShaderType::Vertex },
	{ PIXEL_MAIN, ShaderType::Pixel },
	{ GEOMETRY_MAIN, ShaderType::Geometry }
};

//////////////////////////////////////////////////////////////////////

uint32 optionMap[][2]=
{
	{ DISABLE_OPTIMIZATION,	D3DCOMPILE_SKIP_OPTIMIZATION		},
	{ INCLUDE_DEBUG_INFO,	D3DCOMPILE_DEBUG					},
	{ ERROR_ON_WARNING,		D3DCOMPILE_WARNINGS_ARE_ERRORS		},
	{ PACK_MATRIX_ROW_MAJOR,D3DCOMPILE_PACK_MATRIX_ROW_MAJOR	},
	{ PARTIAL_PRECISION,	D3DCOMPILE_PARTIAL_PRECISION		},
	{ NO_PRESHADER,			D3DCOMPILE_NO_PRESHADER				},
	{ AVOID_FLOW_CONTROL,	D3DCOMPILE_AVOID_FLOW_CONTROL		},
	{ PREFER_FLOW_CONTROL,	D3DCOMPILE_PREFER_FLOW_CONTROL		},
	{ ENABLE_STRICTNESS,	D3DCOMPILE_ENABLE_STRICTNESS		},
	{ IEEE_STRICTNESS,		D3DCOMPILE_IEEE_STRICTNESS			},
	{ RESOURCES_MAY_ALIAS,	D3DCOMPILE_RESOURCES_MAY_ALIAS		}
};

uint32 GetFlags()
{
	uint32 flag = 0;
	for(int i = 0; i < _countof(optionMap); ++i)
	{
		if(options[optionMap[i][0]])
		{
			flag |= optionMap[i][1];
		}
	}
	if(options[OPTIMIZATION_LEVEL])
	{
		switch(atoi(options[OPTIMIZATION_LEVEL].arg))
		{
			case 0: flag |= D3DCOMPILE_OPTIMIZATION_LEVEL0; break;
			case 1: flag |= D3DCOMPILE_OPTIMIZATION_LEVEL1; break;
			case 2: flag |= D3DCOMPILE_OPTIMIZATION_LEVEL2; break;
			case 3: flag |= D3DCOMPILE_OPTIMIZATION_LEVEL3; break;
		}
	}
	return flag;
}

//////////////////////////////////////////////////////////////////////

std::map<ShaderType, HLSLShader *> shaders;

//////////////////////////////////////////////////////////////////////

bool CompileFile(Resource &file, char const *filename, char const *mainFunction, char const *shaderModel, ShaderType type)
{
	ShaderTypeDesc const *desc = ShaderTypeDescFromType(type);
	if(desc == null)
	{
		fprintf(stderr, "Unknown shader type: %d\n", type);
		return false;
	}
	string refname = ToLower(desc->refName);
	string shader = Format("%s_%s", refname.c_str(), shaderModel);	// eg vs_4_0
	wstring fname = WideStringFromString(filename);
	uint flags = GetFlags();
	string output = SetExtension(filename, TEXT(".cso"));
	ID3DBlob *compiledShader;
	ID3DBlob *errors = null;
	if(SUCCEEDED(D3DCompile(file.Data(), file.Size(), filename, null, null, mainFunction, shader.c_str(), flags, 0, &compiledShader, &errors)))
	{
		HLSLShader *s = new HLSLShader(GetFilename(filename));
		DXB(s->Create(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), *desc));
		shaders[s->mShaderTypeDesc.type] = s;

		// append to assembly listing file...

		return true;
	}
	if(errors != null)
	{
		Printer::Output("/*\n%s*/\n", errors->GetBufferPointer());
		printf("%s", errors->GetBufferPointer());
		OutputDebugString((LPCSTR)errors->GetBufferPointer());
	}
	else
	{
		Printer::Output("// Unknown D3DCompileFromFile error\n");
	}
	return false;
}

//////////////////////////////////////////////////////////////////////

enum
{
	success = 0,
	err_args,
	err_nosource,
	err_noshadermodel,
	err_cantcreateheaderfile,
	err_cantwritetoheaderfile,
	err_compilerproblem,
	err_noshaderspecified,
	err_cantloadsourcefile
};

//////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	if(!ParseArgs(argc, argv, options))
	{
		return err_args;
	}

	if(!options[SOURCE_FILE])
	{
		fprintf(stderr, "No source file specified, exiting\n\n");
		PrintUsage();
		return err_nosource;
	}

	if(options[EMBED_BYTECODE])
	{
		printf("!");
	}

	if(!options[SHADER_MODEL_VERSION])
	{
		fprintf(stderr, "Shader model not specified, exiting\n");
		PrintUsage();
		return err_noshadermodel;
	}

	Handle headerFile;
	if(options[HEADER_FILE])
	{
		char const *name = options[HEADER_FILE].arg;
		headerFile = CreateFile(name, GENERIC_WRITE, 0, null, CREATE_ALWAYS, 0, null);
		if(!headerFile.IsValid())
		{
			fprintf(stderr, "Error opening header file, exiting\n");
			return err_cantcreateheaderfile;
		}
	}
	else
	{
		headerFile = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	Printer::SetHeaderFile(headerFile);

	string fileName = GetFilename(options[SOURCE_FILE].arg);

	if(!headerFile.IsValid())
	{
		fprintf(stderr, "Error opening header file for writing, exiting\n");
		return err_cantwritetoheaderfile;
	}

	FileResource sourceFile(options[SOURCE_FILE].arg);
	if(!sourceFile.IsValid())
	{
		fprintf(stderr, "Error loading %s, exiting\n", options[SOURCE_FILE].arg);
		return err_cantloadsourcefile;
	}

	int shadersCompiled = 0;
	for(int i = 0; i < _countof(shaderTypes); ++i)
	{
		ShaderArgType &a = shaderTypes[i];
		if(options[a.arg])
		{
			if(!CompileFile(sourceFile, options[SOURCE_FILE].arg, options[a.arg].arg, options[SHADER_MODEL_VERSION].arg, a.type))
			{
				return err_compilerproblem;
			}
			++shadersCompiled;
		}
	}

	if(shadersCompiled == 0)
	{
		fprintf(stderr, "No shader types specified - output file will be useless!\n");
		return err_noshaderspecified;
	}

	char const *namespaceIn = options[NAMESPACE] ? options[NAMESPACE].arg : null;

	HLSLShader::OutputHeader(fileName.c_str(), namespaceIn);

	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputBlob();
	}
	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputConstBufferNamesAndOffsets();
	}
	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputSamplerNames();
	}
	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputResourceNames();
	}
	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputInputElements();
	}

	using namespace Printer;

	string name = GetFilename(options[SOURCE_FILE].arg);

	OutputCommentLine("Shader struct");
	OutputLine("struct %s : ShaderState", name.c_str());
	OutputIndent("{");
	OutputLine();
	Indent();
	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputHeaderFile();
	}

	OutputCommentLine("Members");

	// now members
	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputMemberVariable();
	}

	OutputLine();
	OutputCommentLine("Constructor");
	OutputLine("%s()", name.c_str());
	Indent("{");
	OutputLine();
	for(uint i = 0; i < NumShaderTypes; ++i)
	{
		char const *name = ShaderTypeDescs[i].name;
		auto f = shaders.find((ShaderType)i);
		if(f == shaders.end())
		{
			OutputLine("Shaders[%s] = null;", name);
		}
		else
		{
			OutputLine("Shaders[%s] = &%s;", name, ToLower(ShaderTypeDescs[i].refName).c_str());;
		}
	}
	UnIndent("}");

	OutputLine();
	OutputCommentLine("Activate");
	OutputLine("void Activate(ID3D11DeviceContext *context)");
	Indent("{");
	OutputLine();
	// Activate
	for(uint i = 0; i < NumShaderTypes; ++i)
	{
		auto f = shaders.find((ShaderType)i);
		if(f == shaders.end())
		{
			OutputLine("context->%sSetShader(null, null, 0);", ShaderTypeDescs[i].refName);
		}
		else
		{
			OutputLine("%s.Activate(context);", ToLower(ShaderTypeDescs[i].refName).c_str());
		}
	}
	UnIndent("}");
	OutputLine();
	OutputLine("void Activate_V(ID3D11DeviceContext *context) override");
	Indent("{");
	OutputLine();
	OutputLine("Activate(context);");
	UnIndent("}");
	UnIndent("};");

	HLSLShader::OutputFooter(fileName.c_str(), namespaceIn);

	return success;
}
