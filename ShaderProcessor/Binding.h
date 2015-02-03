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

using BindingInfoMap_t = std::map<uint, BindingInfo>;

BindingInfo *GetBindingInfo(D3D_SHADER_INPUT_TYPE type);
char const *GetBindingTypeName(BindingInfo::Type bindingType);

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

	virtual void ConstructorOutput()
	{
	}

	static void ClearAllBindings();
	static void ShowAllBindings();

	using ResourceBindingList_t = vector<Binding *>;

	static ResourceBindingList_t ResourceBindingList[BindingInfo::Type::NumBindingTypes];
};

//////////////////////////////////////////////////////////////////////

struct ConstantBufferBinding: Binding
{
	ConstantBufferBinding(HLSLShader *s, D3D11_SHADER_INPUT_BIND_DESC &desc)
		: Binding(s, desc)
	{
	}

	void StaticsOutput() override;
	void MemberOutput() override;
	void ConstructorOutput() override;
};

