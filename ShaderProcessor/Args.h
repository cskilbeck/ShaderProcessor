//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

//#define D3DCOMPILE_PACK_MATRIX_ROW_MAJOR          (1 << 3)
//#define D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR       (1 << 4)
//#define D3DCOMPILE_PARTIAL_PRECISION              (1 << 5)
//#define D3DCOMPILE_FORCE_VS_SOFTWARE_NO_OPT       (1 << 6)
//#define D3DCOMPILE_FORCE_PS_SOFTWARE_NO_OPT       (1 << 7)
//#define D3DCOMPILE_NO_PRESHADER                   (1 << 8)
//#define D3DCOMPILE_AVOID_FLOW_CONTROL             (1 << 9)
//#define D3DCOMPILE_PREFER_FLOW_CONTROL            (1 << 10)
//#define D3DCOMPILE_ENABLE_STRICTNESS              (1 << 11)
//#define D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY (1 << 12)
//#define D3DCOMPILE_IEEE_STRICTNESS                (1 << 13)
//#define D3DCOMPILE_OPTIMIZATION_LEVEL0            (1 << 14)
//#define D3DCOMPILE_OPTIMIZATION_LEVEL1            0
//#define D3DCOMPILE_OPTIMIZATION_LEVEL2            ((1 << 14) | (1 << 15))
//#define D3DCOMPILE_OPTIMIZATION_LEVEL3            (1 << 15)
//#define D3DCOMPILE_RESERVED16                     (1 << 16)
//#define D3DCOMPILE_RESERVED17                     (1 << 17)
//#define D3DCOMPILE_WARNINGS_ARE_ERRORS            (1 << 18)
//#define D3DCOMPILE_RESOURCES_MAY_ALIAS            (1 << 19)

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
	RESOURCES_MAY_ALIAS
};

bool ParseArgs(int argc, char *argv[], vector<option::Option> &options);
void PrintUsage();
