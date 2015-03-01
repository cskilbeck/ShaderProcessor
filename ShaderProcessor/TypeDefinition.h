//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct Binding;

struct TypeDefinition
{
	struct Field
	{
		Field(DX::uint paddingAmount = 0)
			: padding(paddingAmount)
		{
		}

		D3D11_SHADER_VARIABLE_DESC	Variable;
		D3D11_SHADER_TYPE_DESC		Type;
		DX::uint					padding;
	};

	using IntMap = std::unordered_map<DX::string, int>;

	TypeDefinition(ID3D11ShaderReflection *reflection, DX::uint index, Binding *binding);
	~TypeDefinition();
	
	void StaticsOutput(DX::string const &shaderName);
	void MemberOutput(DX::string const &shaderName, DX::uint bindPoint);
	void ConstructorOutput(int index);

	Binding *								mBinding;
	D3D11_SHADER_BUFFER_DESC				mDesc;
	char const *							Name;
	size_t									TotalSizeInBytes;
	DX::Ptr<byte>							Defaults;
	DX::uint								FieldCount;
	DX::uint								mIndex;
	DX::vector<Field *>						mFields;
	IntMap									mFieldIDs;
};

