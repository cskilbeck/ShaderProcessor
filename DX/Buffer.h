//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

enum BufferType
{
	IndexBufferType = D3D11_BIND_INDEX_BUFFER,
	VertexBufferType = D3D11_BIND_VERTEX_BUFFER,
	ConstantBufferType = D3D11_BIND_CONSTANT_BUFFER
};

//////////////////////////////////////////////////////////////////////

enum BufferUsage
{
	DefaultUsage = D3D11_USAGE_DEFAULT,
	StaticUsage = D3D11_USAGE_IMMUTABLE,
	DynamicUsage = D3D11_USAGE_DYNAMIC
};

//////////////////////////////////////////////////////////////////////

enum ReadWriteOption
{
	NotCPUAccessible = 0,
	Readable = D3D11_CPU_ACCESS_READ,
	Writeable = D3D11_CPU_ACCESS_WRITE,
	ReadWriteable = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ
};

//////////////////////////////////////////////////////////////////////

enum MapWaitOption
{
	WaitForGPU = 0,
	DontWaitForGPU = D3D11_MAP_FLAG_DO_NOT_WAIT
};

//////////////////////////////////////////////////////////////////////

template <typename T> struct Buffer
{
	//////////////////////////////////////////////////////////////////////

	Buffer()
	{
	}

	//////////////////////////////////////////////////////////////////////

	Buffer(BufferType type, uint count, T *data = null, BufferUsage usage = DynamicUsage, ReadWriteOption rwOption = NotCPUAccessible)
	{
		Create(type, count, data, usage, rwOption);
	}

	//////////////////////////////////////////////////////////////////////

	HRESULT Create(BufferType type, uint count, T *data = null, BufferUsage usage = DynamicUsage, ReadWriteOption rwOption = NotCPUAccessible)
	{
		mData = data;
		mMapType = (D3D11_MAP)0;
		mUsage = usage;
		mRWOption = rwOption;
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(T) * count;
		desc.BindFlags = type;
		desc.Usage = (D3D11_USAGE)mUsage;
		desc.CPUAccessFlags = mRWOption;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA srd = { data, 0, 0 };
		DX(D3D::Device->CreateBuffer(&desc, &srd, &mBuffer));
		switch(mRWOption)
		{
			case Readable:
				mMapType = D3D11_MAP_READ;
				break;
			case Writeable:
				mMapType = D3D11_MAP_WRITE;
				break;
			case ReadWriteable:
				mMapType = D3D11_MAP_READ_WRITE;
				break;
		}
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////

	T *Map(ID3D11DeviceContext *context, MapWaitOption waitOption = WaitForGPU)
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		if(SUCCEEDED(context->Map(mBuffer, 0, mMapType, waitOption, &msr)))
		{
			return (T *)msr.pData;
		}
		return null;
	}

	//////////////////////////////////////////////////////////////////////

	void UnMap(ID3D11DeviceContext *context)
	{
		context->Unmap(mBuffer, 0);
	}

	//////////////////////////////////////////////////////////////////////

	void Set(ID3D11DeviceContext *context, T *data)
	{
		mData = data;
		Commit();
	}

	//////////////////////////////////////////////////////////////////////

	void Commit(ID3D11DeviceContext *context)
	{
		context->UpdateSubresource(mBuffer, 0, null, mData, 0, 0);
	}

	//////////////////////////////////////////////////////////////////////

	T *mData;
	DXPtr<ID3D11Buffer> mBuffer;
	BufferUsage mUsage;
	ReadWriteOption mRWOption;
	D3D11_MAP mMapType;
};

