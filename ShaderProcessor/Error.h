//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace Error
{
	extern DX::string filename;
	extern DX::uint current_line;
	extern DX::uint error_count;
	extern DX::uint warning_count;
}

//////////////////////////////////////////////////////////////////////

void emit_error(char const *format, ...);
void emit_warning(char const *format, ...);
