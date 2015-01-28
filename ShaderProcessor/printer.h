#pragma once

namespace Printer
{
	void output(char const *format, ...);
	void SetHeaderFile(HANDLE f);
	void SetShader(Shader *s);
	string ShaderName();
}