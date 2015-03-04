//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct SamplerBinding: Binding
{
	SamplerBinding(HLSLShader *s, D3D11_SHADER_INPUT_BIND_DESC &desc);
	bool IsSampler() const override;
	void MemberOutput() override;
	void ConstructorOutput() override;

};

