//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

enum optionIndex
{
	UNKNOWN,
	SOURCE,
	VERTEXMAIN,
	PIXELMAIN,
	GEOMETRYMAIN,
	SMVERSION,
	EMBEDBYTECODE,
	HEADER,
	DISABLEOPTIMIZATION,
	INCLUDEDEBUGINFO,
	ERRORONWARNING,
	INCLUDEPATH
};

bool ParseArgs(int argc, char *argv[], vector<option::Option> &options);
void PrintUsage();
