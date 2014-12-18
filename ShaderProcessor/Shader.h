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

struct Shader
{
	//////////////////////////////////////////////////////////////////////

	using IntMap = std::unordered_map<string, int32>;

	//////////////////////////////////////////////////////////////////////

	uint								mStartIndex;

	DXPtr<ID3D11ShaderReflection>		mReflector;
	D3D11_SHADER_DESC					mShaderDesc;

	vector<SamplerState *>				mSamplers;
	vector<Texture2D *>					mTextures;
	vector<ConstantBuffer *>			mConstantBuffers;
	vector<TextureBuffer *>				mTextureBuffers;

	IntMap								mTextureBufferIDs;
	IntMap								mConstBufferIDs;
	IntMap								mSamplerIDs;
	IntMap								mTextureIDs;

	tstring								mName;

	vector<Reportable *>				mBindings;

	string Name() const
	{
		return StringFromWideString(mName);
	}

	//////////////////////////////////////////////////////////////////////

	Shader(tstring const &filename);
	virtual ~Shader();

	void Output();

	int GetTextureIndex(string const &name) const;
	int GetSamplerIndex(string const &name) const;
	int GetConstantBufferIndex(string const &name) const;
	int GetTextureBufferIndex(string const &name) const;

	uint GetConstantBufferCount() const;
	ConstantBuffer *GetCB(string const &name);
	ConstantBuffer *GetConstantBuffer(int index);

	Reportable *CreateConstantBuffer(D3D11_SHADER_INPUT_BIND_DESC desc);
	Reportable *CreateTextureBinding(D3D11_SHADER_INPUT_BIND_DESC desc);
	Reportable *CreateSamplerBinding(D3D11_SHADER_INPUT_BIND_DESC desc);
	Reportable *CreateTextureBufferBinding(D3D11_SHADER_INPUT_BIND_DESC desc);

	void DeleteConstantBuffers();

	HRESULT Create(void const *blob, size_t size);
	virtual HRESULT Destroy();

	HRESULT CreateBindings();
	Reportable *CreateBinding(D3D11_SHADER_INPUT_BIND_DESC desc);
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

inline ConstantBuffer *Shader::GetCB(string const &name)
{
	int id = GetConstantBufferIndex(name);
	return (id >= 0) ? mConstantBuffers[id] : null;
}

//////////////////////////////////////////////////////////////////////

inline ConstantBuffer *Shader::GetConstantBuffer(int index)
{
	return mConstantBuffers[index];
}

