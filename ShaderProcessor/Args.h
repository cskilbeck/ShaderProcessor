//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

enum optionIndex
{
	UNKNOWN,
	SOURCE_FILE,
	VERTEX_MAIN,
	PIXEL_MAIN,
	GEOMETRY_MAIN,
	SHADER_MODEL_VERSION,
	EMBED_BYTECODE,
	INCLUDE_PATH,
	HEADER_FILE,
	OPTIMIZATION_LEVEL,
	ASSEMBLY_LISTING_FILE,
	DISABLE_OPTIMIZATION,
	INCLUDE_DEBUG_INFO,
	ERROR_ON_WARNING,
	PACK_MATRIX_ROW_MAJOR,
	PARTIAL_PRECISION,
	NO_PRESHADER,
	AVOID_FLOW_CONTROL,
	PREFER_FLOW_CONTROL,
	ENABLE_STRICTNESS,
	IEEE_STRICTNESS,
	RESOURCES_MAY_ALIAS,
	NAMESPACE,
	DATA_FOLDER,
	DATA_ROOT,
	PRAGMA_OPTIONS
};

bool ParseArgs(int argc, char *argv[], vector<option::Option> &options);
void PrintUsage();
