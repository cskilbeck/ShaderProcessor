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
			WriteFile(outputFile, s.data(), (DWORD)s.length(), &wrote, null);
		}
	}

	void WriteString(char const *s)
	{
		if(s)
		{
			DWORD l = (DWORD)strlen(s);
			DWORD wrote;
			WriteFile(outputFile, s, l, &wrote, null);
		}
	}
}

//////////////////////////////////////////////////////////////////////

namespace Printer
{
	//////////////////////////////////////////////////////////////////////

	void Indent(char const *txt, ...)
	{
		if(txt != null)
		{
			char buffer[8192];
			va_list v;
			va_start(v, txt);
			int n = _vsnprintf_s(buffer, sizeof(buffer), txt, v);
			WriteString(indent);
			WriteString(buffer);
		}
		indent += "\t";
	}

	//////////////////////////////////////////////////////////////////////

	void UnIndent(char const *txt, ...)
	{
		indent = indent.substr(0, indent.length() - 1);
		if(txt != null)
		{
			char buffer[8192];
			va_list v;
			va_start(v, txt);
			int n = _vsnprintf_s(buffer, sizeof(buffer), txt, v);
			WriteString(indent);
			WriteString(buffer);
			WriteString("\n");
		}
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
		char buffer[8192];
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

	void OutputCommentLine(char const *format, ...)
	{
		char buffer[8192];
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

	//////////////////////////////////////////////////////////////////////

	void OutputComment(char const *format, ...)
	{
		char buffer[8192];
		va_list v;
		va_start(v, format);
		int n = _vsnprintf_s(buffer, sizeof(buffer), format, v);
		DWORD wrote;
		char const comment2[] = "// ";
		char const crlf2[] = "\n\n";
		WriteString(indent);
		WriteString(comment2);
		WriteFile(outputFile, buffer, n, &wrote, null);
		WriteString(crlf2);
	}

	//////////////////////////////////////////////////////////////////////

	void OutputBinary(void const *data, int sizeOfData)
	{
		char *sep = "";
		uint32 *d = (uint32 *)data;
		for(int i = 0; i < sizeOfData/ 4; ++i)
		{
			Output(sep);
			if((i & 7) == 0)
			{
				OutputLine();
				OutputIndent();
			}
			Output("0x%08x", d[i]);
			sep = ",";
		}
		OutputLine();
	}
}