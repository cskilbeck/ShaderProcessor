//////////////////////////////////////////////////////////////////////
// support everything (Buffers<> especially)
// deal with anonymous cbuffers and tbuffers (index instead of name)
// test alignment/padding etc
// deal with Buffers of structs (no padding)

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
	string shader = Format("%s_%s", desc->refName, shaderModel);	// eg vs_4_0
	wstring fname = WideStringFromString(filename);
	uint flags = 0;
	string output = SetExtension(filename, TEXT(".cso"));
	ID3DBlob *compiledShader;
	ID3DBlob *errors;
	if(D3DCompileFromFile(fname.c_str(), null, D3D_COMPILE_STANDARD_FILE_INCLUDE, mainFunction, shader.c_str(), flags, 0, &compiledShader, &errors) == S_OK)
	{
		Shader s(GetFilename(filename));
		s.Create(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), *desc);
		return true;
	}
	fprintf(stderr, "%s\n", errors->GetBufferPointer());
	return false;
}

//////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	vector<Option> options;
	if(!ParseArgs(argc, argv, options))
	{
		return 1;
	}

	if(!options[SOURCE])
	{
		fprintf(stderr, "No source file specified, exiting\n\n");
		PrintUsage();
		return 1;
	}

	if(!options[SMVERSION])
	{
		fprintf(stderr, "Shader model not specified, exiting\n");
		PrintUsage();
		return 1;
	}

	D3D::Initializer d3d;
	if(!d3d.IsValid)
	{
		fprintf(stderr, "Error initializing D3D, exiting\n");
		return 1;
	}

	Handle headerFile;
	if(options[HEADER])
	{
		char const *name = options[HEADER].arg;
		headerFile = CreateFile(name, GENERIC_WRITE, 0, null, CREATE_ALWAYS, 0, null);
		if(!headerFile.IsValid())
		{
			fprintf(stderr, "Error opening header file, exiting\n");
			return 1;
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
		return 1;
	}

	Shader::OutputHeader(fileName.c_str());
	int shadersCompiled = 0;
	for(int i = 0; i < _countof(shaderTypes); ++i)
	{
		ShaderArgType &a = shaderTypes[i];
		if(options[a.arg])
		{
			if(!CompileFile(options[SOURCE].arg, options[a.arg].arg, options[SMVERSION].arg, a.type))
			{
				return 1;
			}
			++shadersCompiled;
		}
	}
	Shader::OutputFooter();

	if(shadersCompiled == 0)
	{
		fprintf(stderr, "No shader types specified - output file will be useless!\n");
		return 1;
	}

	return 0;
}
