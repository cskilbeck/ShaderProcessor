//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct Binding;

//////////////////////////////////////////////////////////////////////

struct VariableType
{
	static char const *typeNames[];

	VariableType()
		: D3DType(DX::null)
	{
	}

	//////////////////////////////////////////////////////////////////////

	VariableType(ID3D11ShaderReflection *reflection, ID3D11ShaderReflectionType *type, DX::uint index)
		: D3DType(type)
		, mIndex(index)
	{
		using namespace DX;

		D3DType->GetDesc(&Desc);

		if(IsStruct())
		{
			for(uint i = 0; i < Members(); ++i)
			{
				ID3D11ShaderReflectionType *childType = D3DType->GetMemberTypeByIndex(i);
				if(childType == DX::null)
				{
					throw ERROR_NO_DATA;
				}
				members.push_back(new VariableType(reflection, childType, i));
			}
		}

	}

	//////////////////////////////////////////////////////////////////////

	bool IsStruct() const
	{
		return Desc.Class == D3D_SVC_STRUCT;
	}

	DX::uint Members() const
	{
		return Desc.Members;
	}

	bool IsArray() const
	{
		return Rows() > 1;
	}

	char const *Name() const
	{
		return Desc.Name;
	}

	DX::uint Columns() const
	{
		return Desc.Columns;
	}

	DX::uint Rows() const
	{
		return Desc.Rows;
	}

	DX::uint Elements() const
	{
		return Desc.Elements;
	}

	D3D_SHADER_VARIABLE_CLASS Class() const
	{
		return Desc.Class;
	}

	D3D_SHADER_VARIABLE_TYPE Type() const
	{
		return Desc.Type;
	}

	char const *TypeName() const
	{
		return typeNames[Type()];
	}

	void OutputDeclaration()
	{
		using namespace Printer;
		if(IsStruct())
		{
			Output("struct");
			OutputLine();
			OutputLine("{");
			Indent();
			for(auto m : members)
			{
				OutputIndent();
				m->OutputDeclaration();
				Output(" %s;", D3DType->GetMemberTypeName(mIndex));
				OutputLine();
			}
			UnIndent();
			OutputIndent("} ");
		}
		else
		{
			Output("%s%s%d ", TypeName(), IsArray() ? DX::Format("%dx", Rows()).c_str() : "", Columns());
		}
	}

	//////////////////////////////////////////////////////////////////////

	ID3D11ShaderReflectionType *D3DType;
	D3D11_SHADER_TYPE_DESC		Desc;
	DX::uint					mIndex;
	DX::vector<VariableType *>	members;	// so it's a tree of structs...
};

//////////////////////////////////////////////////////////////////////

struct Variable
{
	Variable(ID3D11ShaderReflection *reflection, ID3D11ShaderReflectionVariable *var)
		: D3DVar(var)
	{
		D3DVar->GetDesc(&Desc);
		Type = new VariableType(reflection, var->GetType(), 0);
	}

	byte *Defaults() const
	{
		return (byte *)Desc.DefaultValue;
	}

	char const *Name() const
	{
		return Desc.Name;
	}

	size_t Size() const
	{
		return Desc.Size;
	}

	DX::uint StartOffset() const
	{
		return Desc.StartOffset;
	}

	DX::uint EndOffset() const
	{
		return (DX::uint)(StartOffset() + Size());
	}

	void OutputDefinition()
	{
		using namespace DX;
		using namespace Printer;
		OutputIndent();
		Type->OutputDeclaration();
		Output("%s;%s", Name(), Padding().c_str());
	}

	DX::string Padding()
	{
		return (padding != 0) ? DX::Format("\t\t\t\tprivate: byte pad%d[%d]; public:", padID++, padding).c_str() : "";
	}

	static void ResetPaddingID()
	{
		padID = 0;
	}

	ID3D11ShaderReflectionVariable *	D3DVar;
	D3D11_SHADER_VARIABLE_DESC			Desc;
	VariableType *						Type;
	DX::uint							padding;

	static DX::uint						padID;
};

//////////////////////////////////////////////////////////////////////

struct TypeDefinition
{
	using IntMap = std::unordered_map<DX::string, int>;

	TypeDefinition(ID3D11ShaderReflection *reflection, DX::uint index);
	~TypeDefinition();
	
	void StaticsOutput(DX::string const &shaderName);
	void MemberOutput(DX::string const &shaderName, DX::uint bindPoint);
	void ConstructorOutput(int index);

	Binding *								mBinding;		// only set for top level ones (const buffers, basically)
	D3D11_SHADER_BUFFER_DESC				mDesc;
	char const *							Name;
	DX::uint								TotalSizeInBytes;
	DX::uint								FieldCount;
	DX::Ptr<byte>							mDefaultBuffer;
	DX::uint								mIndex;
	DX::vector<Variable *>					mVariables;

	byte *DefaultsBuffer()
	{
		using namespace DX;

		if(mDefaultBuffer == null)
		{
			mDefaultBuffer.reset(new byte[mDesc.Size]);
			memset(mDefaultBuffer.get(), 0, mDesc.Size);
		}
		return mDefaultBuffer.get();
	}
};
