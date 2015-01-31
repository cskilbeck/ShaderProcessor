#pragma once


struct HLSLShader;

namespace Printer
{
	void output(char const *format, ...);
	void SetHeaderFile(HANDLE f);
	void SetShader(HLSLShader *s);
	string ShaderName();
}