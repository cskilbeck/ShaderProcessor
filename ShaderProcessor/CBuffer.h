//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct ConstantBufferBinding: Binding
{
	ConstantBufferBinding(Shader *s, D3D11_SHADER_INPUT_BIND_DESC &desc)
		: Binding(s, desc)
	{
	}

	void StaticsOutput() override;
	void MemberOutput() override;
	void ConstructorOutput() override;
};

//////////////////////////////////////////////////////////////////////

struct ResourceBinding: Binding
{
	ResourceBinding(Shader *s, D3D11_SHADER_INPUT_BIND_DESC &desc)
		: Binding(s, desc)
	{
	}
};

//////////////////////////////////////////////////////////////////////

struct SamplerBinding: Binding
{
	SamplerBinding(Shader *s, D3D11_SHADER_INPUT_BIND_DESC &desc)
		: Binding(s, desc)
	{
	}
};

