//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

SamplerBinding::SamplerBinding(HLSLShader *s, D3D11_SHADER_INPUT_BIND_DESC &desc)
	: Binding(s, desc)
{
	// Try to get the defaults for the Sampler variable here?
}

bool SamplerBinding::IsSampler() const
{
	return true;
}

void SamplerBinding::MemberOutput()
{
	Printer::OutputLine("Sampler *%s;", mDesc.Name);
}

void SamplerBinding::ConstructorOutput()
{
	Printer::OutputLine(", %s(null)", mDesc.Name);
}