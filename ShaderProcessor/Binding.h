//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct Shader;

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
};

//////////////////////////////////////////////////////////////////////

using BindingInfoMap_t = std::map<uint, BindingInfo>;

//////////////////////////////////////////////////////////////////////

struct Binding
{
	Shader *						mShader;
	D3D11_SHADER_INPUT_BIND_DESC	mDesc;
	TypeDefinition *				definition;

	char const *Name()
	{
		return mDesc.Name;
	}

	string TypeName();

	Binding(Shader *s, D3D11_SHADER_INPUT_BIND_DESC &desc);

	virtual ~Binding()
	{
	}

	virtual void StaticsOutput()
	{
	}

	virtual void MemberOutput()
	{
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


