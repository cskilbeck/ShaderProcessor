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

	void StaticsOutput() override
	{
	}

	void MemberOutput() override
	{
		printf("\t\tTexture2D *%s;\n", mDesc.Name);
	}

	void ConstructorOutput() override
	{
		printf("%s(null)\n", mDesc.Name);
	}
};

