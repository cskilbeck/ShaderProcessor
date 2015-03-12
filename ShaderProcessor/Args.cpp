//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <iostream>

//////////////////////////////////////////////////////////////////////

using option::Option;
vector<Option> options;

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
		string names("' must be one of: ");
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

	static option::ArgStatus OptimizationLevel(Option const &option, bool msg)
	{
		char const *sep = "";
		string names("' must be one of:\n");
		for(uint i = 0; i < 4; ++i)
		{
			if(atoi(option.arg) == i)
			{
				return option::ARG_OK;
			}
			names += Format("%s%d", sep, i);
			sep = ",";
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
	{ SOURCE_FILE, 0, "", "source", Arg::NonEmpty, "--source=sourceFile (required). Specify the source HLSL file." },
	{ SHADER_MODEL_VERSION, 0, "", "sm", Arg::ShaderModel, "--sm=shadermodel (eg 4_0)(required). Specify the Shader Model (4_0, 4_1, 5_0, 4_0_level_9_1, 4_0_level_9_3)." },
	{ HEADER_FILE, 0, "", "header", Arg::NonEmpty, "--header=headerFile (optional). Where to output the header file to. If not specified, goes to stdout." },
	{ EMBED_BYTECODE, 0, "", "embedObject", Arg::None, "--embedObject (optional). Embed the shader object byte code in the header file." },
	{ DISABLE_OPTIMIZATION, 0, "", "disableOptimization", Arg::None, "--disableOptimization (optional). Turn off the Shader Compiler Optimizer." },
	{ INCLUDE_DEBUG_INFO, 0, "", "debugInfo", Arg::None, "--includeDebugInfo (optional). Switch this on to enable graphics debugging in Visual Studio." },
	{ ERROR_ON_WARNING, 0, "", "errorOnWarning", Arg::None, "--errorOnWarning (optional). Return an error code if warnings are generated." },
	{ VERTEX_MAIN, 0, "", "vs", Arg::NonEmpty, "--vs=vsMainFunction (optional). Specify a Vertex Shader main function, if there is one." },
	{ PIXEL_MAIN, 0, "", "ps", Arg::NonEmpty, "--ps=psMainFunction (optional). Specify a Pixel Shader main function, if there is one." },
	{ GEOMETRY_MAIN, 0, "", "gs", Arg::NonEmpty, "--gs=gsMainFunction (optional). Specify a Geometry Shader main function, if there is one." },
	{ INCLUDE_PATH, 0, "", "includePath", Arg::NonEmpty, "--includePath=\"path1;path2\" (optional). Set the include path." },
	{ PACK_MATRIX_ROW_MAJOR, 0, "", "packRowMajor", Arg::None, "--packRowMajor (optional). Pack matrices in row major order (default is column major)." },
	{ PARTIAL_PRECISION, 0, "", "partialPrecision", Arg::None, "--partialPrecision (optional). Use partial precision on some hardware." },
	{ NO_PRESHADER, 0, "", "noPreshader", Arg::None, "--noPreshader (optional). Disable preshader generation for known constants." },
	{ AVOID_FLOW_CONTROL, 0, "", "avoidFlowControl", Arg::None, "--avoidFlowControl (optional). Hint to the compiler to avoid flow control instructions." },
	{ PREFER_FLOW_CONTROL, 0, "", "preferFlowControl", Arg::None, "--preferFlowControl (optional). Hint to the compiler to prefer flow control instructions." },
	{ ENABLE_STRICTNESS, 0, "", "enableStrictness", Arg::None, "--enableStrictness (optional). Enable strict mode." },
	{ IEEE_STRICTNESS, 0, "", "ieeeStrictness", Arg::None, "--ieeeStrictness (optional). Enable IEEE strictness mode." },
	{ RESOURCES_MAY_ALIAS, 0, "", "resourcesMayAlias", Arg::None, "--resourcesMayAlias (optional). UAVs and SRVs may alias for cs_5_0." },
	{ OPTIMIZATION_LEVEL, 0, "", "optimizationLevel", Arg::OptimizationLevel, "--optimizationLevel=[0|1|2|3] (optional). Specify level of optimization (3 = most)." },
	{ ASSEMBLY_LISTING_FILE, 0, "", "assemblyListingFile", Arg::NonEmpty, "--assemblyListingFile=filename (optional). Output assembly listing on successful compilation." },
	{ NAMESPACE, 0, "", "namespace", Arg::NonEmpty, "--namespace=Namespace (optional). Enclose Shader definition in a namespace." },
	{ DATA_FOLDER, 0, "", "data_folder", Arg::NonEmpty, "--data_folder=Data" },
	{ DATA_ROOT, 0, "", "data_root", Arg::NonEmpty, "--data_root=..\\Projdir" },
	{ PRAGMA_OPTIONS, 0, "", "pragma_options", Arg::None, "--pragma_options (optional). Show #pragma option documentation." },
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

	printf("\nOptions specified:\n\n");
	for(auto &o : options)
	{
		if(o.desc != null)
		{
			string s = "";
			if(o.arg != null)
			{
				s = Format("=%s", o.arg);
			}
			printf("%s%s\n", o.name, s.c_str());
		}
	}
}

//////////////////////////////////////////////////////////////////////

uint32 optionMap[][2] =
{
	{ DISABLE_OPTIMIZATION, D3DCOMPILE_SKIP_OPTIMIZATION },
	{ INCLUDE_DEBUG_INFO, D3DCOMPILE_DEBUG },
	{ ERROR_ON_WARNING, D3DCOMPILE_WARNINGS_ARE_ERRORS },
	{ PACK_MATRIX_ROW_MAJOR, D3DCOMPILE_PACK_MATRIX_ROW_MAJOR },
	{ PARTIAL_PRECISION, D3DCOMPILE_PARTIAL_PRECISION },
	{ NO_PRESHADER, D3DCOMPILE_NO_PRESHADER },
	{ AVOID_FLOW_CONTROL, D3DCOMPILE_AVOID_FLOW_CONTROL },
	{ PREFER_FLOW_CONTROL, D3DCOMPILE_PREFER_FLOW_CONTROL },
	{ ENABLE_STRICTNESS, D3DCOMPILE_ENABLE_STRICTNESS },
	{ IEEE_STRICTNESS, D3DCOMPILE_IEEE_STRICTNESS },
	{ RESOURCES_MAY_ALIAS, D3DCOMPILE_RESOURCES_MAY_ALIAS }
};

//////////////////////////////////////////////////////////////////////

uint32 GetCompilerOptionFlags()
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

int CheckArgs(int argc, char *argv[])
{
	// parse the args
	if(!ParseArgs(argc, argv, options))
	{
		emit_error("Usage");
		return err_args;
	}

	if(options[PRAGMA_OPTIONS])
	{
		OutputPragmaDocs();
		return err_okbutquit;
	}

	if(!options[SOURCE_FILE])
	{
		emit_error("No source file specified");
		PrintUsage();
		return err_nosource;
	}

	if(!options[SHADER_MODEL_VERSION])
	{
		emit_error("Shader model not specified");
		PrintUsage();
		return err_noshadermodel;
	}

	// check input file exists
	if(!FileExists(options[SOURCE_FILE].arg))
	{
		emit_error("Source file doesn't exist");
		return err_nosource;
	}
	return success;
}

