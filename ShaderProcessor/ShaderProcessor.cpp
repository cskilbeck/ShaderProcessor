//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

using namespace DirectX;

//////////////////////////////////////////////////////////////////////

using option::Option;

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
	{ PIXELMAIN, ShaderType::Pixel }
};

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
	if(D3DCompileFromFile(fname.c_str(), null, D3D_COMPILE_STANDARD_FILE_INCLUDE, mainFunction, shader.c_str(), flags, 0, &compiledShader, &errors) == S_OK)
	{
		HLSLShader s(GetFilename(filename));
		s.Create(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), *desc);
		return true;
	}
	Printer::Output("/*\n");
	if(errors != null)
	{
		Printer::Output("%s\n", errors->GetBufferPointer());
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
	vector<Option> options;
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

	HLSLShader::OutputHeader(fileName.c_str());
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
	HLSLShader::OutputFooter(fileName.c_str());

	if(shadersCompiled == 0)
	{
		fprintf(stderr, "No shader types specified - output file will be useless!\n");
		return err_noshaderspecified;
	}

	return success;
}
