//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////
// Define the HLSL Vector and Matrix types

namespace HLSL
{
	#pragma pack(push, 4)

	template <typename t, int n> using vector = t[n];
	template <typename t, int r, int c> using matrix = vector<t, c>[r];

	#pragma pack(pop)

	using Bool = uint32;
	using Int = int32;
	using UInt = uint32;
	using DWord = uint32;
	using Float = float;

	enum
	{
		X = 0,
		Y = 1,
		Z = 2,
		W = 3
	};

	#pragma push_macro("join2")
	#pragma push_macro("join3")
	#pragma push_macro("join4")
	#pragma push_macro("def_vec")
	#pragma push_macro("def_mat_col")
	#pragma push_macro("def_mat")


	#define join2(x, y) x ## y
	#define join3(x, y, z) join2(x, y) ## z
	#define join4(x, y, z, w) join3(x, y, z) ## w

#define VEC1(X) template <typename X> struct join2(X, 1) { X x; operator X[1]() { return &x; } };
#define VEC2(X) template <typename X> struct join2(X, 2) { X x, y; operator X[2]() { return &x; } };
#define VEC3(X) template <typename X> struct join2(X, 3) { X x, y, z; operator X[3]() { return &x; } };
#define VEC4(X) template <typename X> struct join2(X, 4) { X x, y, z, w; operator X[4]() { return &x; } };

#define MAT1(X) template <typename X> struct join4(X, 1, x, 1) { join2 }

#define VEC(X) VEC1(X) VEC2(X) VEC3(X) VEC4(X)

	VEC(Int)
	VEC(UInt)
	VEC(DWord)
	VEC(Bool)
	VEC(Float)

	#define def_vec(T) \
		using join2(T, 1) = vector<T, 1>;	\
		using join2(T, 2) = vector<T, 2>;	\
		using join2(T, 3) = vector<T, 3>;	\
		using join2(T, 4) = vector<T, 4>;

	#define def_mat_col(T, C) \
		using join4(T, 1, x, C) = matrix<T, 1, C>; \
		using join4(T, 2, x, C) = matrix<T, 2, C>; \
		using join4(T, 3, x, C) = matrix<T, 3, C>; \
		using join4(T, 4, x, C) = matrix<T, 4, C>;

	#define def_mat(T) \
		def_mat_col(T, 1) \
		def_mat_col(T, 2) \
		def_mat_col(T, 3) \
		def_mat_col(T, 4)

	def_vec(Bool)
	def_vec(Int)
	def_vec(UInt)
	def_vec(DWord)
	def_vec(Float)

	def_mat(Bool)
	def_mat(Int)
	def_mat(UInt)
	def_mat(DWord)
	def_mat(Float)

	#pragma pop_macro("join2")
	#pragma pop_macro("join3")
	#pragma pop_macro("join4")
	#pragma pop_macro("def_vec")
	#pragma pop_macro("def_mat_col")
	#pragma pop_macro("def_mat")

	// use weak & declspec(selectany) for defaults...

} // namespace HLSL

//////////////////////////////////////////////////////////////////////

struct Shader
{
	//////////////////////////////////////////////////////////////////////

	using IntMap = std::unordered_map<string, int32>;

	//////////////////////////////////////////////////////////////////////

	uint								mStartIndex;
	vector<ID3D11Buffer *>				mBuffers;

	DXPtr<ID3D11ShaderReflection>		mReflector;
	D3D11_SHADER_DESC					mShaderDesc;

	vector<ID3D11SamplerState *>		mSamplers;	// client fills these in
	vector<ID3D11ShaderResourceView *>	mTextures;

	vector<ConstantBuffer *>			mConstantBuffers;
	vector<TextureBuffer *>				mTextureBuffers;

	IntMap								mTextureBufferIDs;
	IntMap								mConstBufferIDs;
	IntMap								mSamplerIDs;
	IntMap								mTextureIDs;

	tstring								mName;

	//////////////////////////////////////////////////////////////////////

	Shader(tstring const &filename);
	virtual ~Shader();

	int GetTextureIndex(string const &name) const;
	int GetSamplerIndex(string const &name) const;
	int GetConstantBufferIndex(string const &name) const;

	void SetTexture(int index, ID3D11ShaderResourceView *t);
	void SetTexture(string const &name, ID3D11ShaderResourceView *t);

	void SetSampler(int index, ID3D11SamplerState *s);
	void SetSampler(string const &name, ID3D11SamplerState *s);

	uint GetConstantBufferCount() const;
	ConstantBuffer *GetCB(string const &name);
	ConstantBuffer *GetConstantBuffer(int index);

	HRESULT CreateConstantBuffer(D3D11_SHADER_INPUT_BIND_DESC desc);
	HRESULT CreateTextureBinding(D3D11_SHADER_INPUT_BIND_DESC desc);
	HRESULT CreateSamplerBinding(D3D11_SHADER_INPUT_BIND_DESC desc);
	HRESULT CreateTextureBuffer(D3D11_SHADER_INPUT_BIND_DESC desc);

	void DeleteConstantBuffers();

	HRESULT Create(void const *blob, size_t size);
	virtual HRESULT Destroy();

	HRESULT CreateBindings();
	HRESULT CreateBinding(D3D11_SHADER_INPUT_BIND_DESC desc);
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

inline void Shader::SetTexture(int index, ID3D11ShaderResourceView *t)
{
	if(index >= 0)
	{
		mTextures[index] = t;
	}
}

//////////////////////////////////////////////////////////////////////

inline void Shader::SetTexture(string const &name, ID3D11ShaderResourceView *t)
{
	SetTexture(GetTextureIndex(name), t);
}

//////////////////////////////////////////////////////////////////////

inline void Shader::SetSampler(int index, ID3D11SamplerState *s)
{
	if(index >= 0)
	{
		mSamplers[index] = s;
	}
}

//////////////////////////////////////////////////////////////////////

inline void Shader::SetSampler(string const &name, ID3D11SamplerState *s)
{
	SetSampler(GetSamplerIndex(name), s);
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

