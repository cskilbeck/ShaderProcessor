//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

void ConstantBufferBinding::StaticsOutput()
{
	definition->StaticsOutput(mShader->Name());
}

//////////////////////////////////////////////////////////////////////

void ConstantBufferBinding::MemberOutput()
{
	definition->MemberOutput(mShader->Name(), mDesc.BindPoint);
}

//////////////////////////////////////////////////////////////////////

void ConstantBufferBinding::ConstructorOutput()
{
	definition->ConstructorOutput(mDesc.BindPoint);
}

void ConstantBufferBinding::CreateOutput()
{
	definition->CreateOutput(mDesc.BindPoint);
}

void ConstantBufferBinding::ReleaseOutput()
{
	definition->ReleaseOutput();
}