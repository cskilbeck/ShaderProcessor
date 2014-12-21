//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct ConstantBuffer : Reportable
{
	struct Parameter
	{
		Parameter(uint paddingAmount = 0)
			: padding(paddingAmount)
		{
		}

		D3D11_SHADER_VARIABLE_DESC	Variable;
		D3D11_SHADER_TYPE_DESC		Type;
		uint						padding;
	};

	using IntMap = std::unordered_map<string, int>;

	ConstantBuffer(Shader *s, D3D11_SHADER_INPUT_BIND_DESC desc, ID3D11ShaderReflectionConstantBuffer *b);
	~ConstantBuffer() override;

	void StaticsOutput() override;
	void MemberOutput() override;
	void ConstructorOutput() override;

	D3D11_SHADER_BUFFER_DESC				mDesc;
	char const *							Name;
	size_t									TotalSizeInBytes;
	Ptr<byte>								Defaults;
	ID3D11ShaderReflectionConstantBuffer *	mReflectionCB;
	vector<Parameter *>						mParameters;
	IntMap									mParameterIDs;

};
