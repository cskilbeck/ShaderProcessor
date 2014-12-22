//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct SamplerState: Binding
{
	SamplerState(Shader *s, D3D11_SHADER_INPUT_BIND_DESC desc)
		: Binding(s, desc)
	{
	}

	~SamplerState() override
	{
	}

	void StaticsOutput() override
	{
	}

	void MemberOutput() override
	{
		printf("\t\tSamplerState *%s;\n", mDesc.Name);
	}

	void ConstructorOutput() override
	{
		printf("%s(null)\n", mDesc.Name);
	}
};

