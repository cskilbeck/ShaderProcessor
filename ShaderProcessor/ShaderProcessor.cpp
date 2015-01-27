//////////////////////////////////////////////////////////////////////
// detect ps/vs etc and make it derive from the right thing
// support everything (Buffers<> especially)
// fix texturebuffers by doing the '3 types' of thing split
// deal with anonymous cbuffers and tbuffers (index instead of name)
// test alignment/padding etc
// deal with Buffers of structs (no padding)

#include "stdafx.h"
#include "optionparser.h"
//#include "D:\\output.h"

using namespace DirectX;

//////////////////////////////////////////////////////////////////////

enum optionIndex
{
	UNKNOWN,
	SOURCE,
	VERTEXMAIN,
	PIXELMAIN,
	SMVERSION
};

//////////////////////////////////////////////////////////////////////

struct Arg: public option::Arg
{
	//////////////////////////////////////////////////////////////////////

	static void printError(const char* msg1, const option::Option& opt, const char* msg2)
	{
		fprintf(stderr, "ERROR: %s", msg1);
		fwrite(opt.name, opt.namelen, 1, stderr);
		fprintf(stderr, "%s", msg2);
	}

	//////////////////////////////////////////////////////////////////////

	static option::ArgStatus ShaderModel(option::Option const &option, bool msg)
	{
		string names("' must be one of:\n");
		for(uint i = 0; i < _countof(ShaderModelName); ++i)
		{
			if(strcmp(option.arg, ShaderModelName[i]) == 0)
			{
				return option::ARG_OK;
			}
			names += Format("%s\n", ShaderModelName[i]);
		}
		if(msg)
		{
			printError("Option '", option, names.c_str());
		}
		return option::ARG_ILLEGAL;
	}

	//////////////////////////////////////////////////////////////////////

	static option::ArgStatus NonEmpty(const option::Option& option, bool msg)
	{
		if(option.arg != null && option.arg[0] != 0)
		{
			return option::ARG_OK;
		}

		if(msg)
		{
			printError("Option '", option, "' requires an argument");
		}
		return option::ARG_ILLEGAL;
	}
};

//////////////////////////////////////////////////////////////////////

option::Descriptor const usage[] =
{
	{ UNKNOWN,		0,	"",	"",			option::Arg::None,	"USAGE: sp [options]\n\nOptions:"		},
	{ SOURCE,		0,	"",	"source",	Arg::NonEmpty,		"--source sourceFile (required)"		},
	{ VERTEXMAIN,	0,	"",	"vs",		Arg::NonEmpty,		"--vs vsMainFunction (optional)"		},
	{ PIXELMAIN,	0,	"",	"ps",		Arg::NonEmpty,		"--ps psMainFunction (optional)"		},
	{ SMVERSION,	0,	"",	"sm",		Arg::ShaderModel,	"--sm shadermodel (eg 4_0)(required)"	},
	{ 0, 0, null, null, null, null }
};

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

int _tmain(int argc, char *argv[])
{
	if(argc > 0)
	{
		--argc;
		++argv;
	}
	option::Stats stats(usage, argc, argv);
	vector<option::Option> options(stats.options_max);
	vector<option::Option> buffer(stats.buffer_max);
	option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);

	int rc = 0;
	if(D3D::Open())
	{
		string fileName = GetFilename(options[SOURCE].arg);
		Shader::OutputHeader(fileName.c_str());
		for(int i = 0; i < _countof(shaderTypes); ++i)
		{
			ShaderArgType &a = shaderTypes[i];
			if(options[a.arg])
			{
				if(!CompileFile(options[SOURCE].arg, options[a.arg].arg, options[SMVERSION].arg, a.type))
				{
					rc = 1;
				}
			}
		}
		Shader::OutputFooter();
	}
	D3D::Close();
	return rc;
}
