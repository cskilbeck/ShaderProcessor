//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct VertexShader
{
};

//////////////////////////////////////////////////////////////////////

struct PixelShader
{
};

//////////////////////////////////////////////////////////////////////

struct InputField
{
	char const *name;		// eg Float
	uint sizeInBytes;		// eg 16
	uint elementCount;		// eg 4

	string GetName() const
	{
		return Format("%s%d", name, elementCount);
	}
};

struct Shader
{
	//////////////////////////////////////////////////////////////////////

	using IntMap = std::unordered_map<string, int32>;

	//////////////////////////////////////////////////////////////////////

	uint								mStartIndex;

	DXPtr<ID3D11ShaderReflection>		mReflector;
	D3D11_SHADER_DESC					mShaderDesc;

	// These can be referenced by ConstantBuffers, TextureBuffers, StructuredBuffers etc
	vector<TypeDefinition *>			mDefinitions;
	IntMap								mDefinitionIDs;	// map names of Definitions to the mDefinitions vector

	vector<SamplerState *>				mSamplers;
	vector<Texture2D *>					mTextures;
	vector<ConstantBufferBinding *>		mConstantBuffers;
	vector<TextureBuffer *>				mTextureBuffers;
	vector<D3D11_INPUT_ELEMENT_DESC>	mInputElements;
	vector<InputField>					mInputFields;

	IntMap								mTextureBufferIDs;
	IntMap								mConstBufferIDs;
	IntMap								mSamplerIDs;
	IntMap								mTextureIDs;

	string								mName;

	vector<Binding *>					mBindings;

	string Name() const
	{
		return mName;
	}

	//////////////////////////////////////////////////////////////////////

	Shader(tstring const &filename);
	virtual ~Shader();

	void Output();
	void OutputInputElements();
	void OutputInputStruct();

	int GetTextureIndex(string const &name) const;
	int GetSamplerIndex(string const &name) const;
	int GetConstantBufferIndex(string const &name) const;
	int GetTextureBufferIndex(string const &name) const;

	uint GetConstantBufferCount() const;
	ConstantBufferBinding *GetCB(string const &name);
	ConstantBufferBinding *GetConstantBuffer(int index);

	HRESULT CreateInputLayout();
	HRESULT CreateDefinitions();

	Binding *CreateConstantBuffer(D3D11_SHADER_INPUT_BIND_DESC desc);
	Binding *CreateTextureBinding(D3D11_SHADER_INPUT_BIND_DESC desc);
	Binding *CreateSamplerBinding(D3D11_SHADER_INPUT_BIND_DESC desc);
	Binding *CreateTextureBufferBinding(D3D11_SHADER_INPUT_BIND_DESC desc);

	void DeleteConstantBuffers();

	HRESULT Create(void const *blob, size_t size);
	virtual HRESULT Destroy();

	HRESULT CreateBindings();
	Binding *CreateBinding(D3D11_SHADER_INPUT_BIND_DESC desc);
	void DestroyBindings();
};

//////////////////////////////////////////////////////////////////////

static inline int GetIndex(string const &name, Shader::IntMap const &map)
{
	auto i = map.find(name);
	return (i == map.end()) ? -1 : i->second;
}

//////////////////////////////////////////////////////////////////////

inline int Shader::GetTextureIndex(string const &name) const
{
	return GetIndex(name, mTextureIDs);
}

//////////////////////////////////////////////////////////////////////

inline int Shader::GetSamplerIndex(string const &name) const
{
	return GetIndex(name, mSamplerIDs);
}

//////////////////////////////////////////////////////////////////////

inline int Shader::GetConstantBufferIndex(string const &name) const
{
	return GetIndex(name, mConstBufferIDs);
}

//////////////////////////////////////////////////////////////////////

inline uint Shader::GetConstantBufferCount() const
{
	return (uint)mConstantBuffers.size();
}

//////////////////////////////////////////////////////////////////////

inline ConstantBufferBinding *Shader::GetCB(string const &name)
{
	int id = GetConstantBufferIndex(name);
	return (id >= 0) ? mConstantBuffers[id] : null;
}

//////////////////////////////////////////////////////////////////////

inline ConstantBufferBinding *Shader::GetConstantBuffer(int index)
{
	return mConstantBuffers[index];
}

