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

	bool IsResource() const override
	{
		return true;
	}

	void MemberOutput() override
	{
		Printer::output("Texture2D *%s;", mDesc.Name);
	}

	void ConstructorOutput() override
	{
		Printer::output("%s(null)", mDesc.Name);
	}
};

//////////////////////////////////////////////////////////////////////

struct SamplerBinding: Binding
{
	SamplerBinding(Shader *s, D3D11_SHADER_INPUT_BIND_DESC &desc)
		: Binding(s, desc)
	{
	}

	bool IsSampler() const override
	{
		return true;
	}

	void MemberOutput() override
	{
		Printer::output("SamplerState *%s;", mDesc.Name);
	}

	void ConstructorOutput() override
	{
		Printer::output("%s(null)", mDesc.Name);
	}

};

