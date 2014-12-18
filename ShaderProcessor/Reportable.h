//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct Shader;

struct Reportable
{
	Shader *mShader;
	D3D11_SHADER_INPUT_BIND_DESC mDesc;

	Reportable(Shader *s, D3D11_SHADER_INPUT_BIND_DESC desc)
		: mShader(s)
		, mDesc(desc)
	{
	}

	virtual ~Reportable()
	{
	}

	virtual void StaticsOutput() = 0;			// defaults and nametable
	virtual void MemberOutput() = 0;			// definition and declaration of member variable
	virtual void ConstructorOutput() = 0;		// construction
};

