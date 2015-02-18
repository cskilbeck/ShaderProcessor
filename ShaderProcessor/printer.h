#pragma once


struct HLSLShader;

namespace Printer
{
	void Indent(char const *txt = nullptr, ...);
	void UnIndent(char const *txt = nullptr, ...);
	void Output(char const *format, ...);
	void OutputLine(char const *format, ...);
	void OutputLine();
	void OutputCommentLine(char const *format, ...);
	void OutputComment(char const *format, ...);
	void OutputIndent(char const *after = nullptr);
	void OutputBinary(void const *data, int sizeOfData);
	void SetHeaderFile(HANDLE f);
	void SetShader(HLSLShader *s);
	DX::string ShaderName();
}