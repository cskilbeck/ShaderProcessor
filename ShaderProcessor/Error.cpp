//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

namespace Error
{
	string filename;
	uint current_line;
	uint error_count = 0;
	uint warning_count = 0;
}

using namespace Error;

//////////////////////////////////////////////////////////////////////

void emit_error(char const *format, ...)
{
	va_list v;
	va_start(v, format);
	string e = Format_V(format, v);
	printf("%s(%d): error S0000: %s\n", filename.c_str(), current_line, e.c_str());
	++error_count;
}

//////////////////////////////////////////////////////////////////////

void emit_warning(char const *format, ...)
{
	va_list v;
	va_start(v, format);
	string e = Format_V(format, v);
	printf("%s(%d): warning T0000: %s\n", filename.c_str(), current_line, e.c_str());
	++warning_count;
}
