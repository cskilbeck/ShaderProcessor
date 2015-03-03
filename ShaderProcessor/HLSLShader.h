//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

enum ShaderType
{
	Vertex = 0,
	Hull = 1,
	Domain = 2,
	Geometry = 3,
	Pixel = 4,
	Compute = 5,
	NumShaderTypes = 6
};

enum StorageType
{
	Invalid_type,
	Float_type,
	Half_type,
	Int_type,
	UInt_type,
	Short_type,
	UShort_type,
	Byte_type,
	SByte_type,
	Norm16_type,
	Norm8_type,
	UNorm16_type,
	UNorm8_type,
	Typeless32_type,
	Typeless16_type,
	Typeless8_type,
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
	sm4_0_level_9_1 = 0,
	sm4_0_level_9_3 = 1,
	sm4_0 = 2,
	sm4_1 = 3,
	sm5_0 = 4
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
	{ Vertex,	"Vertex",	"VS", m4_0_level_9_1 |	m4_0_level_9_3 |	m4_0 |	m4_1 |	m5_0	},
	{ Hull,		"Hull",		"HS",														m5_0	},
	{ Domain,	"Domain",	"DS",														m5_0	},
	{ Geometry,	"Geometry",	"GS",										m4_0 |	m4_1 |	m5_0	},
	{ Pixel,	"Pixel",	"PS", m4_0_level_9_1 |	m4_0_level_9_3 |	m4_0 |	m4_1 |	m5_0	},
	{ Compute,	"Compute",	"CS",										m4_0 |	m4_1 |	m5_0	}
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
	StorageType storageType;	// Float etc
	uint elementCount;			// eg 4
	string varName;				// eg Position (everything in the Semantic name after the last _ )
	uint arraySize;
	uint inputSlot;

	string GetTypeName() const;	// eg Float4

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
	struct BindingRun
	{
		uint16 mBindPoint;
		uint16 mBindCount;
	};

	vector<BindingRun> mBindingRuns[BindingInfo::Type::NumBindingTypes];

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

	static void OutputHeader(char const *filename, char const *namespace_);
	static void OutputFooter(char const *filename, char const *namespace_);
	
	HLSLShader(tstring const &filename);
	virtual ~HLSLShader();

	void OutputHeaderFile();
	void OutputConstBufferNamesAndOffsets();
	void OutputResourceNames();
	void OutputSamplerNames();
	void OutputShaderStruct();
	void OutputBindingRuns();
	void OutputConstBufferMembers();
	void OutputSamplerMembers();
	void OutputResourceMembers();
	void OutputConstructor();
	void OutputInputElements();
	void OutputInputStruct();
	void OutputBlob();
	void OutputMemberVariable();

	string Name() const;
	string RefName() const;
	char const *ShaderTypeName() const;
	void *GetConstantBuffer(char const *name);

	string VSTag();

	int GetTextureIndex(string const &name) const;
	int GetSamplerIndex(string const &name) const;
	int GetConstantBufferIndex(string const &name) const;

	uint GetConstantBufferCount() const;
	ConstantBufferBinding *GetCB(string const &name);
	ConstantBufferBinding *GetConstantBuffer(int index);

	HRESULT Create(void const *blob, size_t size, ShaderTypeDesc const &desc);
	HRESULT CreateInputLayout();
	HRESULT CreateBindings();
	virtual HRESULT Destroy();

	Binding *CreateBinding(D3D11_SHADER_INPUT_BIND_DESC desc);

	void Dump();

	void AddBinding(Binding *b);
};

//////////////////////////////////////////////////////////////////////

inline void *HLSLShader::GetConstantBuffer(char const *name)
{
	static std::unordered_map<char const *, void *> const m =
	{
		{ "VertConstants", null }
	};
	auto p = m.find(name);
	return (p == m.end()) ? null : p->second;
}

//////////////////////////////////////////////////////////////////////

static inline int GetIndex(string const &name, HLSLShader::IntMap const &map)
{
	auto i = map.find(name);
	return (i == map.end()) ? -1 : i->second;
}

//////////////////////////////////////////////////////////////////////

inline string HLSLShader::RefName() const
{
	return string(mShaderTypeDesc.refName);
}

//////////////////////////////////////////////////////////////////////

inline string HLSLShader::Name() const
{
	return mName + "_" + RefName();
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

