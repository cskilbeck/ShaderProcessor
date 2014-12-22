//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct TypeDefinition
{
	struct Field
	{
		Field(uint paddingAmount = 0)
			: padding(paddingAmount)
		{
		}

		D3D11_SHADER_VARIABLE_DESC	Variable;
		D3D11_SHADER_TYPE_DESC		Type;
		uint						padding;
	};

	using IntMap = std::unordered_map<string, int>;

	TypeDefinition(ID3D11ShaderReflection *reflection, uint index);
	~TypeDefinition();

	void StaticsOutput(string const &shaderName);
	void MemberOutput(string const &shaderName);

	D3D11_SHADER_BUFFER_DESC				mDesc;
	char const *							Name;
	size_t									TotalSizeInBytes;
	Ptr<byte>								Defaults;
	uint									FieldCount;
	ID3D11ShaderReflectionConstantBuffer *	mReflectionCB;
	vector<Field *>							mFields;
	IntMap									mFieldIDs;
};

