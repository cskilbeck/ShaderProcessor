#pragma once


struct HLSLShader;

namespace Printer
{
	void Indent();
	void UnIndent();
	void Output(char const *format, ...);
	void OutputLine(char const *format, ...);
	void OutputLine();
	void OutputComment(char const *format, ...);
	void OutputIndent(char const *after = nullptr);

	void SetHeaderFile(HANDLE f);
	void SetShader(HLSLShader *s);
	DX::string ShaderName();
}