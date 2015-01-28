//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <iostream>

//////////////////////////////////////////////////////////////////////

using option::Option;

//////////////////////////////////////////////////////////////////////

struct Arg: public option::Arg
{
	//////////////////////////////////////////////////////////////////////

	static void printError(const char* msg1, const Option& opt, const char* msg2)
	{
		fprintf(stderr, "ERROR: %s", msg1);
		fwrite(opt.name, opt.namelen, 1, stderr);
		fprintf(stderr, "%s", msg2);
	}

	//////////////////////////////////////////////////////////////////////

	static option::ArgStatus ShaderModel(Option const &option, bool msg)
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

	static option::ArgStatus Flag(Option const &option, bool msg)
	{
		return option::ARG_OK;
	}

	//////////////////////////////////////////////////////////////////////

	static option::ArgStatus NonEmpty(const Option& option, bool msg)
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
	{ UNKNOWN, 0, "", "", option::Arg::None, "USAGE: sp options\n\nShader Processor\n\nGenerates header files from shaders.\nCan compile multiple shader types from a single file.\n\nOptions:" },
	{ SOURCE, 0, "", "source", Arg::NonEmpty, "--source=sourceFile (required). Specify the source HLSL file." },
	{ VERTEXMAIN, 0, "", "vs", Arg::NonEmpty, "--vs=vsMainFunction (optional). Specify a Vertex Shader main function, if there is one." },
	{ PIXELMAIN, 0, "", "ps", Arg::NonEmpty, "--ps=psMainFunction (optional). Specify a Pixel Shader main function, if there is one." },
	{ SMVERSION, 0, "", "sm", Arg::ShaderModel, "--sm=shadermodel (eg 4_0)(required). Specify the Shader Model (4_0, 4_1, 5_0, 4_0_level_9_1, 4_0_level_9_3)" },
	{ HEADER, 0, "", "header", Arg::NonEmpty, "--header=headerFile. Where to output the header file to. If not specified, goes to stdout." },
	{ EMBEDBYTECODE, 0, "", "embedbytecode", Arg::Flag, "--embedbytecode (optional). Embed the shader byte code in the header file." },
	{ 0, 0, null, null, null, null }
};

//////////////////////////////////////////////////////////////////////

bool ParseArgs(int argc, char *argv[], vector<Option> &options)
{
	options.clear();
	if(argc > 0)
	{
		--argc;
		++argv;
	}
	option::Stats stats(usage, argc, argv);
	options.resize(stats.options_max);
	vector<Option> buffer(stats.buffer_max);
	option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);
	return parse.error() == false;
}

//////////////////////////////////////////////////////////////////////

void PrintUsage()
{
	option::printUsage(std::cerr, usage);
}
