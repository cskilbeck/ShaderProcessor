//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct SamplerBinding: Binding
{
	SamplerBinding(HLSLShader *s, D3D11_SHADER_INPUT_BIND_DESC &desc)
		: Binding(s, desc)
	{
	}

	bool IsSampler() const override
	{
		return true;
	}

	void MemberOutput() override
	{
		Printer::output("Sampler *%s;", mDesc.Name);
	}

	void ConstructorOutput() override
	{
		Printer::output("%s(null)", mDesc.Name);
	}

};

