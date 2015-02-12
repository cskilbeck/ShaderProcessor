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
	{ VERTEXMAIN, ShaderType::Vertex },
	{ PIXELMAIN, ShaderType::Pixel },
	{ GEOMETRYMAIN, ShaderType::Geometry }
};

//////////////////////////////////////////////////////////////////////

std::map<ShaderType, HLSLShader *> shaders;

//////////////////////////////////////////////////////////////////////

bool CompileFile(char const *filename, char const *mainFunction, char const *shaderModel, ShaderType type)
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
	uint flags = 0;
	string output = SetExtension(filename, TEXT(".cso"));
	ID3DBlob *compiledShader;
	ID3DBlob *errors = null;

	if(options[INCLUDEDEBUGINFO])
	{
		flags |= D3DCOMPILE_DEBUG;
	}

	if(options[DISABLEOPTIMIZATION])
	{
		flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	}
	if(options[ERRORONWARNING])
	{
		flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
	}

	// etc etc...

	if(D3DCompileFromFile(fname.c_str(), null, D3D_COMPILE_STANDARD_FILE_INCLUDE, mainFunction, shader.c_str(), flags, 0, &compiledShader, &errors) == S_OK)
	{
		HLSLShader *s = new HLSLShader(GetFilename(filename));
		DXB(s->Create(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), *desc));
		shaders[s->mShaderTypeDesc.type] = s;
		return true;
	}
	Printer::Output("/*\n");
	if(errors != null)
	{
		Printer::Output("%s\n", errors->GetBufferPointer());
		printf("%s", errors->GetBufferPointer());
	}
	else
	{
		Printer::Output("Unknown D3DCompileFromFile error\n");
	}
	Printer::Output("*/\n");
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
};

//////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	if(!ParseArgs(argc, argv, options))
	{
		return err_args;
	}

	if(!options[SOURCE])
	{
		fprintf(stderr, "No source file specified, exiting\n\n");
		PrintUsage();
		return err_nosource;
	}

	if(options[EMBEDBYTECODE])
	{
		printf("!");
	}

	if(!options[SMVERSION])
	{
		fprintf(stderr, "Shader model not specified, exiting\n");
		PrintUsage();
		return err_noshadermodel;
	}

	Handle headerFile;
	if(options[HEADER])
	{
		char const *name = options[HEADER].arg;
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

	string fileName = GetFilename(options[SOURCE].arg);

	if(!headerFile.IsValid())
	{
		fprintf(stderr, "Error opening header file for writing, exiting\n");
		return err_cantwritetoheaderfile;
	}

	int shadersCompiled = 0;
	for(int i = 0; i < _countof(shaderTypes); ++i)
	{
		ShaderArgType &a = shaderTypes[i];
		if(options[a.arg])
		{
			if(!CompileFile(options[SOURCE].arg, options[a.arg].arg, options[SMVERSION].arg, a.type))
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

	// set up the Printer
	// Output Header
	HLSLShader::OutputHeader(fileName.c_str());

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
	// Output Structure Header

	using namespace Printer;

	string name = GetFilename(options[SOURCE].arg);

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
			OutputLine("Shaders[%s] = &%sShader;", name, name);
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
			OutputLine("%sShader.Activate(context);", ShaderTypeDescs[i].name);
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
	HLSLShader::OutputFooter(fileName.c_str());

	return success;
}
