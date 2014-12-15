//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct Buffer
{
	Buffer()
	{
	}

	~Buffer()
	{
	}
};

//////////////////////////////////////////////////////////////////////

struct TextureBuffer
{

};

//////////////////////////////////////////////////////////////////////

struct ConstantBuffer
{
	ConstantBuffer();
	~ConstantBuffer();

	struct Parameter
	{
		D3D11_SHADER_VARIABLE_DESC	Variable;
		D3D11_SHADER_TYPE_DESC		Type;
	};

	HRESULT		Create(ID3D11ShaderReflectionConstantBuffer *b);
	byte *		GetAddressAndSizeOf(string const &name, size_t &size);
	byte *		AddressOf(string const &name);
	byte *		GetBuffer();

	char const *	Name;
	size_t			TotalSizeInBytes;
	Ptr<byte>		Buffer;
	ID3D11Buffer *	mConstantBuffer;

private:

	using ParamMap = std::unordered_map<string, Parameter *>;

	ParamMap	Parameters;

	Parameter *	GetParameter(string const &name);
};

//////////////////////////////////////////////////////////////////////

inline ConstantBuffer::Parameter *ConstantBuffer::GetParameter(string const &name)
{
	auto i = Parameters.find(name);
	return (i != Parameters.end()) ? i->second : null;
}

//////////////////////////////////////////////////////////////////////

inline byte *ConstantBuffer::GetAddressAndSizeOf(string const &name, size_t &size)
{
	byte *addr = null;
	Parameter *p = GetParameter(name);
	if(p != null)
	{
		size = p->Variable.Size;
		addr = Buffer.get() + p->Variable.StartOffset;
	}
	return addr;
}

//////////////////////////////////////////////////////////////////////

inline byte *ConstantBuffer::AddressOf(string const &name)
{
	Parameter *p = GetParameter(name);
	return (p != null) ? Buffer.get() + p->Variable.StartOffset : null;
}

//////////////////////////////////////////////////////////////////////

inline byte *ConstantBuffer::GetBuffer()
{
	return Buffer.get();
}