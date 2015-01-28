//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

enum optionIndex
{
	UNKNOWN,
	SOURCE,
	VERTEXMAIN,
	PIXELMAIN,
	SMVERSION,
	EMBEDBYTECODE,
	HEADER
};

bool ParseArgs(int argc, char *argv[], vector<option::Option> &options);
void PrintUsage();
