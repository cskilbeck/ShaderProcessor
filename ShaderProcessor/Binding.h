//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct HLSLShader;

//////////////////////////////////////////////////////////////////////

struct BindingInfo
{
	enum Type
	{
		SamplerState = 0,
		Resource = 1,
		ConstantBuffer = 2,
		NumBindingTypes = 3
	};

	bool		NeedsDefinition;
	Type		BindingType;
	char const *Name;
	char const *TypeName;
};

//////////////////////////////////////////////////////////////////////

using namespace DX;

BindingInfo *GetBindingInfo(D3D_SHADER_INPUT_TYPE type);

//////////////////////////////////////////////////////////////////////

struct Binding
{
	HLSLShader *					mShader;
	D3D11_SHADER_INPUT_BIND_DESC	mDesc;
	TypeDefinition *				definition;

	char const *Name()
	{
		return mDesc.Name;
	}

	string TypeName();

	Binding(HLSLShader *s, D3D11_SHADER_INPUT_BIND_DESC &desc);

	virtual ~Binding()
	{
	}

	virtual void StaticsOutput()
	{
	}

	virtual void MemberOutput()
	{
	}

	virtual bool IsResource() const
	{
		return false;
	}

	virtual bool IsSampler() const
	{
		return false;
	}

	virtual bool IsConstBuffer() const
	{
		return false;
	}

	virtual void ConstructorOutput()
	{
	}

	virtual void CreateOutput()
	{
	}

	virtual void ReleaseOutput()
	{
	}
};

//////////////////////////////////////////////////////////////////////

struct ConstantBufferBinding: Binding
{
	ConstantBufferBinding(HLSLShader *s, D3D11_SHADER_INPUT_BIND_DESC &desc);

	bool IsConstBuffer() const override
	{
		return true;
	}

	void StaticsOutput() override;
	void MemberOutput() override;
	void ConstructorOutput() override;
	void CreateOutput() override;
	void ReleaseOutput() override;
};

