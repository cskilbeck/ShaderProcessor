//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct ResourceBinding: Binding
{
	ResourceBinding(HLSLShader *s, D3D11_SHADER_INPUT_BIND_DESC &desc)
		: Binding(s, desc)
	{
	}

	bool IsResource() const override
	{
		return true;
	}

	void MemberOutput() override
	{
		Printer::output("Texture *%s;", mDesc.Name);
	}

	void ConstructorOutput() override
	{
		Printer::output("%s(null)", mDesc.Name);
	}
};

