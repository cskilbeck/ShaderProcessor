//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct TypelessBuffer
	{
		//////////////////////////////////////////////////////////////////////

		TypelessBuffer()
			: mOwnData(false)
		{
		}

		//////////////////////////////////////////////////////////////////////

		virtual ~TypelessBuffer()
		{
			CleanUp();
		}

		//////////////////////////////////////////////////////////////////////

		void CleanUp()
		{
			if(mOwnData)
			{
				Delete(mData);
			}
		}

		//////////////////////////////////////////////////////////////////////

		TypelessBuffer(BufferType type, uint32 size, byte *data = null, BufferUsage usage = DefaultUsage, ReadWriteOption rwOption = NotCPUAccessible)
		{
			Create(type, size, data, usage, rwOption);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(BufferType type, uint32 size, byte *data = null, BufferUsage usage = DefaultUsage, ReadWriteOption rwOption = NotCPUAccessible)
		{
			mSize = size;
			mData = data;
			if(mData == null)
			{
				mData = new byte[size];
				mOwnData = true;
			}
			mMapType = (D3D11_MAP)0;
			mUsage = usage;
			mRWOption = rwOption;
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = mSize;
			desc.BindFlags = type;
			desc.Usage = (D3D11_USAGE)mUsage;
			desc.CPUAccessFlags = mRWOption;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			D3D11_SUBRESOURCE_DATA srd = { mData, 0, 0 };
			DXR(DX::Device->CreateBuffer(&desc, &srd, &mBuffer));
			switch(mRWOption)
			{
				case Readable:
					mMapType = D3D11_MAP_READ;
					break;
				case Writeable:
					mMapType = D3D11_MAP_WRITE_DISCARD;
					break;
				case ReadWriteable:
					mMapType = D3D11_MAP_READ_WRITE;
					break;
			}
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		byte * const Map(ID3D11DeviceContext *context, MapWaitOption waitOption = WaitForGPU)
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			if(SUCCEEDED(context->Map(mBuffer, 0, mMapType, waitOption, &msr)))
			{
				return (byte * const)msr.pData;
			}
			return null;
		}

		//////////////////////////////////////////////////////////////////////

		void UnMap(ID3D11DeviceContext *context)
		{
			context->Unmap(mBuffer, 0);
		}

		//////////////////////////////////////////////////////////////////////

		void Set(ID3D11DeviceContext *context, byte *data)
		{
			CleanUp();
			mData = data;
			mOwnData = false;
			Commit(context);
		}

		//////////////////////////////////////////////////////////////////////

		void Update(ID3D11DeviceContext *context, byte *data)
		{
			context->UpdateSubresource(mBuffer, 0, null, data, 0, 0);
		}

		//////////////////////////////////////////////////////////////////////

		uint32 Size() const
		{
			return mSize;
		}

		//////////////////////////////////////////////////////////////////////

		ID3D11Buffer * &Handle()
		{
			return mBuffer.p;
		}

		//////////////////////////////////////////////////////////////////////

		byte *Data() const
		{
			return mData;
		}

		//////////////////////////////////////////////////////////////////////

		void Commit(ID3D11DeviceContext *context)
		{
			context->UpdateSubresource(mBuffer, 0, null, mData, 0, 0);
		}

		//////////////////////////////////////////////////////////////////////

	protected:

		byte *				mData;
		uint32				mSize;
		DXPtr<ID3D11Buffer>	mBuffer;
		BufferUsage			mUsage;
		ReadWriteOption		mRWOption;
		D3D11_MAP			mMapType;
		bool				mOwnData;
	};

	//////////////////////////////////////////////////////////////////////

	template <typename T> struct Buffer: TypelessBuffer
	{
		//////////////////////////////////////////////////////////////////////

		Buffer()
			: TypelessBuffer()
		{
		}

		//////////////////////////////////////////////////////////////////////

		Buffer(BufferType type, uint count, T *data = null, BufferUsage usage = DefaultUsage, ReadWriteOption rwOption = NotCPUAccessible)
		{
			Create(type, count, data, usage, rwOption);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(BufferType type, uint count, T *data = null, BufferUsage usage = DefaultUsage, ReadWriteOption rwOption = NotCPUAccessible)
		{
			return TypelessBuffer::Create(type, count * sizeof(T), (byte *)data, usage, rwOption);
		}

		//////////////////////////////////////////////////////////////////////

		T * const Map(ID3D11DeviceContext *context, MapWaitOption waitOption = WaitForGPU)
		{
			return (T * const)TypelessBuffer::Map(context, waitOption);
		}

		//////////////////////////////////////////////////////////////////////

		void Set(ID3D11DeviceContext *context, T *data)
		{
			TypelessBuffer::Set(content, (byte *)data);
		}

		//////////////////////////////////////////////////////////////////////

		T &operator[](uint index)
		{
			return ((T *)mData)[index];
		}

		//////////////////////////////////////////////////////////////////////

		uint32 Count() const
		{
			return mSize / sizeof(T);
		}

		//////////////////////////////////////////////////////////////////////

		T *Data() const
		{
			return (T *)TypelessBuffer::Data();
		}
	};

}