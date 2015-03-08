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

//////////////////////////////////////////////////////////////////////

enum
{
	success = 0,
	err_warnings_issued = MININT,
	err_args,
	err_nosource,
	err_noshadermodel,
	err_cantcreateheaderfile,
	err_cantwritetoheaderfile,
	err_compilerproblem,
	err_noshaderspecified,
	err_cantloadsourcefile,
	err_unknownpragma,
	err_malformedpragma,
	err_duplicatepragma,
	err_malformedsemanticname,
	err_badsemantic,
	err_unknownkey
};

