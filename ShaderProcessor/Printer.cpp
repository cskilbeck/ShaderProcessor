//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

namespace
{
	HANDLE outputFile;
	string shaderName;
	HLSLShader *shader;
	string indent;

	void WriteString(string const &s)
	{
		if(!s.empty())
		{
			DWORD wrote;
			WriteFile(outputFile, s.data(), s.length(), &wrote, null);
		}
	}

	void WriteString(char const *s)
	{
		if(s)
		{
			size_t l = strlen(s);
			DWORD wrote;
			WriteFile(outputFile, s, l, &wrote, null);
		}
	}
}

//////////////////////////////////////////////////////////////////////

namespace Printer
{
	//////////////////////////////////////////////////////////////////////

	void Indent()
	{
		indent += "\t";
	}

	//////////////////////////////////////////////////////////////////////

	void UnIndent()
	{
		indent = indent.substr(0, indent.length() - 1);
	}

	//////////////////////////////////////////////////////////////////////

	void SetHeaderFile(HANDLE f)
	{
		outputFile = f;
	}

	//////////////////////////////////////////////////////////////////////

	void SetShader(HLSLShader *s)
	{
		shader = s;
	}

	//////////////////////////////////////////////////////////////////////

	string ShaderName()
	{
		return shader->Name();
	}

	//////////////////////////////////////////////////////////////////////

	void OutputIndent(char const *after)
	{
		WriteString(indent);
		if(after != null)
		{
			WriteString(after);
		}
	}

	//////////////////////////////////////////////////////////////////////

	void Output(char const *format, ...)
	{
		char buffer[1024];
		va_list v;
		va_start(v, format);
		int n = _vsnprintf_s(buffer, sizeof(buffer), format, v);
		DWORD wrote;
		WriteFile(outputFile, buffer, n, &wrote, null);
	}

	//////////////////////////////////////////////////////////////////////

	void OutputLine()
	{
		WriteString("\n");
	}

	//////////////////////////////////////////////////////////////////////

	void OutputLine(char const *format, ...)
	{
		char buffer[1024];
		va_list v;
		va_start(v, format);
		int n = _vsnprintf_s(buffer, sizeof(buffer), format, v);
		DWORD wrote;
		WriteString(indent);
		WriteFile(outputFile, buffer, n, &wrote, null);
		OutputLine();
	}

	//////////////////////////////////////////////////////////////////////

	void OutputComment(char const *format, ...)
	{
		char buffer[1024];
		va_list v;
		va_start(v, format);
		int n = _vsnprintf_s(buffer, sizeof(buffer), format, v);
		DWORD wrote;
		char const comment[] = "//////////////////////////////////////////////////////////////////////\n";
		char const comment2[] = "// ";
		char const crlf2[] = "\n\n";
		WriteString(indent);
		WriteString(comment);
		WriteString(indent);
		WriteString(comment2);
		WriteFile(outputFile, buffer, n, &wrote, null);
		WriteString(crlf2);
	}
}