//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct Texture2D: Binding
{
	Texture2D(Shader *s, D3D11_SHADER_INPUT_BIND_DESC desc)
		: Binding(s, desc)
	{
	}

	~Texture2D() override
	{
	}

};

