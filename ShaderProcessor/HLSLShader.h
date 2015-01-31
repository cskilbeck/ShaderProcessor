//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

enum StorageType
{
	Float,
	Half,
	Int,
	UInt,
	Short,
	UShort,
	Byte,
	SByte,
	Norm16,
	Norm8,
	UNorm16,
	UNorm8,
	Typeless32,
	Typeless16,
	Typeless8
};

//////////////////////////////////////////////////////////////////////

enum ShaderType
{
	Vertex = 0,
	Hull = 1,
	Domain = 2,
	Geometry = 3,
	Pixel = 4,
	Compute = 5
};

//////////////////////////////////////////////////////////////////////

struct ShaderTypeDesc
{
	ShaderType	type;
	char const *name;
	char const *refName;
	uint32 shaderModelsAllowed;
};

// can be:			vs	hs	ds	gs	ps	cs
// 4_0_level_9_1	##	..	..	..	##	..
// 4_0_level_9_3	##	..	..	..	##	..
// 4_0				##	..	..	##	##	##
// 4_1				##	..	..	##	##	##
// 5_0				##	##	##	##	##	##

//////////////////////////////////////////////////////////////////////

enum ShaderModel: uint32
{
	sm4_0_level_9_1 = 1,
	sm4_0_level_9_3 = 2,
	sm4_0 = 3,
	sm4_1 = 4,
	sm5_0 = 5
};

//////////////////////////////////////////////////////////////////////

enum ShaderModelMask: uint32
{
	m4_0_level_9_1 = 1 << sm4_0_level_9_1,
	m4_0_level_9_3 = 1 << sm4_0_level_9_3,
	m4_0 = 1 << sm4_0,
	m4_1 = 1 << sm4_1,
	m5_0 = 1 << sm5_0
};

//////////////////////////////////////////////////////////////////////

extern char const WEAKSYM *ShaderModelName[] =
{
	"4_0_level_9_1",
	"4_0_level_9_3",
	"4_0",
	"4_1",
	"5_0"
};

//////////////////////////////////////////////////////////////////////

extern ShaderTypeDesc WEAKSYM ShaderTypeDescs[] =
{
	{ Vertex,	"Vertex",	"vs", m4_0_level_9_1 |	m4_0_level_9_3 |	m4_0 |	m4_1 |	m5_0	},
	{ Hull,		"Hull",		"hs",														m5_0	},
	{ Domain,	"Domain",	"ds",														m5_0	},
	{ Geometry,	"Geometry",	"gs",										m4_0 |	m4_1 |	m5_0	},
	{ Pixel,	"Pixel",	"ps", m4_0_level_9_1 |	m4_0_level_9_3 |	m4_0 |	m4_1 |	m5_0	},
	{ Compute,	"Compute",	"cs",										m4_0 |	m4_1 |	m5_0	}
};

//////////////////////////////////////////////////////////////////////

inline ShaderTypeDesc const *ShaderTypeDescFromType(ShaderType type)
{
	for(uint i = 0; i < _countof(ShaderTypeDescs); ++i)
	{
		if(ShaderTypeDescs[i].type == type)
		{
			return &ShaderTypeDescs[i];
		}
	}
	return null;
}

//////////////////////////////////////////////////////////////////////

inline ShaderTypeDesc const &ShaderTypeDescFromReferenceName(char const *referenceName)
{
	for(uint i = 0; i < _countof(ShaderTypeDescs); ++i)
	{
		if(strcmp(ShaderTypeDescs[i].refName, referenceName) == 0)
		{
			return ShaderTypeDescs[i];
		}
	}
}

//////////////////////////////////////////////////////////////////////

struct InputType
{
	char const *name;
	int fieldCount;	// if this is 0, infer from input type
	StorageType storageType;
};

//////////////////////////////////////////////////////////////////////

struct DXGI_FormatDescriptor
{
	DXGI_FORMAT format;
	char const *name;
	uint fields;
	uint bitSize;
	StorageType storageType;

	uint GetTotalSizeInBits() const
	{
		return fields * bitSize;
	}
};

//////////////////////////////////////////////////////////////////////

struct InputField
{
	StorageType storageType;		// Float etc
	uint elementCount;			// eg 4
	string varName;				// eg Position (everything in the Semantic name after the last _)

	string GetTypeName() const;  // eg Float4

	string GetDeclaration() const
	{
		return Format("%s %s", GetTypeName().c_str(), varName.c_str());
	}
};

//////////////////////////////////////////////////////////////////////

struct ConstantBufferBinding;
struct ResourceBinding;
struct SamplerBinding;

struct HLSLShader
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

	vector<D3D11_INPUT_ELEMENT_DESC>	mInputElements;
	vector<InputField>					mInputFields;

	IntMap								mTextureBufferIDs;
	IntMap								mConstBufferIDs;
	IntMap								mSamplerIDs;
	IntMap								mTextureIDs;

	ShaderType							mShaderType;
	ShaderTypeDesc						mShaderTypeDesc;

	void const *						mBlob;
	size_t								mSize;

	string								mName;

	vector<Binding *>					mBindings;

	vector<ResourceBinding *>			mResourceBindings;
	vector<SamplerBinding *>			mSamplerBindings;
	vector<ConstantBufferBinding *>		mConstantBufferBindings;

	int									mConstBuffers;
	int									mSamplers;
	int									mResources;

	static void OutputHeader(char const *filename);
	static void OutputFooter();
	
	string Name() const
	{
		return string(mShaderTypeDesc.refName) + "_" + mName;
	}

	//////////////////////////////////////////////////////////////////////

	void *GetConstantBuffer(char const *name)
	{
		static std::unordered_map<char const *, void *> const m =
		{
			{ "VertConstants", null }
		};
		auto p = m.find(name);
		return (p == m.end()) ? null : p->second;
	}

	HLSLShader(tstring const &filename);
	virtual ~HLSLShader();

	void Output();
	void OutputInputElements();
	void OutputInputStruct();
	void OutputBlob();

	int GetTextureIndex(string const &name) const;
	int GetSamplerIndex(string const &name) const;
	int GetConstantBufferIndex(string const &name) const;

	uint GetConstantBufferCount() const;
	ConstantBufferBinding *GetCB(string const &name);
	ConstantBufferBinding *GetConstantBuffer(int index);

	HRESULT CreateInputLayout();
	HRESULT CreateDefinitions();

	HRESULT Create(void const *blob, size_t size, ShaderTypeDesc const &desc);
	virtual HRESULT Destroy();

	HRESULT CreateBindings();
	Binding *CreateBinding(D3D11_SHADER_INPUT_BIND_DESC desc);
};

//////////////////////////////////////////////////////////////////////

static inline int GetIndex(string const &name, HLSLShader::IntMap const &map)
{
	auto i = map.find(name);
	return (i == map.end()) ? -1 : i->second;
}

//////////////////////////////////////////////////////////////////////

inline int HLSLShader::GetTextureIndex(string const &name) const
{
	return GetIndex(name, mTextureIDs);
}

//////////////////////////////////////////////////////////////////////

inline int HLSLShader::GetSamplerIndex(string const &name) const
{
	return GetIndex(name, mSamplerIDs);
}

//////////////////////////////////////////////////////////////////////

inline int HLSLShader::GetConstantBufferIndex(string const &name) const
{
	return GetIndex(name, mConstBufferIDs);
}

//////////////////////////////////////////////////////////////////////

inline uint HLSLShader::GetConstantBufferCount() const
{
	return (uint)mConstantBufferBindings.size();
}

//////////////////////////////////////////////////////////////////////

inline ConstantBufferBinding *HLSLShader::GetCB(string const &name)
{
	int id = GetConstantBufferIndex(name);
	return (id >= 0) ? mConstantBufferBindings[id] : null;
}

//////////////////////////////////////////////////////////////////////

inline ConstantBufferBinding *HLSLShader::GetConstantBuffer(int index)
{
	return mConstantBufferBindings[index];
}

