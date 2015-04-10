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
//			Release();
		}

		//////////////////////////////////////////////////////////////////////

		void Release()
		{
			if(mOwnData)
			{
				Delete(mData);
			}
			mBuffer.Release();
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT CreateBuffer(BufferType type, uint32 size, byte *data = null, BufferUsage usage = DynamicUsage, ReadWriteOption rwOption = Writeable)
		{
			mSize = size;
			mData = data;
			if(mData == null && usage == StaticUsage)
			{
				mData = new byte[size];
				mOwnData = true;
			}
			mMapType = (D3D11_MAP)0;
			mUsage = usage;
			if(mUsage == StaticUsage && rwOption != NotCPUAccessible)
			{
				INFO("INFO: Setting CPU Access option to NotCPUAccessible\n");
				rwOption = NotCPUAccessible;
			}
			mRWOption = rwOption;
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = mSize;
			desc.BindFlags = type;
			desc.Usage = (D3D11_USAGE)mUsage;
			desc.CPUAccessFlags = mRWOption;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			D3D11_SUBRESOURCE_DATA srd = { mData, 0, 0 };
			D3D11_SUBRESOURCE_DATA *psrd = (mData == null) ? null : &srd;
			DXR(DX::Device->CreateBuffer(&desc, psrd, &mBuffer));
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

		HRESULT Map(ID3D11DeviceContext *context, byte * &p, MapWaitOption waitOption = WaitForGPU)
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			DXR(context->Map(mBuffer, 0, mMapType, waitOption, &msr));
			p = (byte *)msr.pData;
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		void UnMap(ID3D11DeviceContext *context)
		{
			context->Unmap(mBuffer, 0);
		}

		//////////////////////////////////////////////////////////////////////

		void UnMap()
		{
			DX::Context->Unmap(mBuffer, 0);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Set(ID3D11DeviceContext *context, byte *data)
		{
			Release();
			mData = data;
			mOwnData = false;
			DXR(Commit(context));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Update(ID3D11DeviceContext *context, byte *data)
		{
			byte *p;
			DXR(Map(context, p));
			memcpy(p, data, mSize);
			UnMap(context);
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		uint32 Size() const
		{
			return mSize;
		}

		//////////////////////////////////////////////////////////////////////

		ID3D11Buffer * Handle() const
		{
			return mBuffer.p;
		}

		//////////////////////////////////////////////////////////////////////

		byte *Data() const
		{
			return mData;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Commit(ID3D11DeviceContext *context)
		{
			byte *p;
			DXR(Map(context, p));
			assert(p != null);
			memcpy(p, mData, mSize);
			UnMap(context);
			return S_OK;
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

		template <typename T> struct MappedResource
		{
			Buffer<T> *buffer;
			T * resource;

			MappedResource(Buffer<T> *buf)
				: buffer(buf)
			{
				DXI(buffer->Map(resource));
			}

			MappedResource(MappedResource &other)
			{
				Release();
				buffer = other.buffer;
				DXI(resource = buffer->Map());
			}

			~MappedResource()
			{
				Release();
			}

			void Release()
			{
				if(resource != null)
				{
					DXI(buffer->UnMap(DX::Context));
					resource = null;
				}
			}

			T & operator [] (uint index)
			{
				return resource[index];
			}

			T * operator -> ()
			{
				return resource;
			}
		};

		//////////////////////////////////////////////////////////////////////

		uint mSizeOf;

		Buffer()
			: TypelessBuffer()
			, mSizeOf(0)
		{
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT CreateBuffer(BufferType type, uint count, T *data = null, BufferUsage usage = DefaultUsage, ReadWriteOption rwOption = NotCPUAccessible)
		{
			mSizeOf = sizeof(T);
			return TypelessBuffer::CreateBuffer(type, count * sizeof(T), (byte *)data, usage, rwOption);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Map(ID3D11DeviceContext *context, T * &ptr, MapWaitOption waitOption = WaitForGPU)
		{
			DXR(TypelessBuffer::Map(context, (byte * &)ptr, waitOption));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Map(T * &ptr, MapWaitOption waitOption = WaitForGPU)
		{
			DXR(Map(DX::Context, ptr, waitOption));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////
		// TODO (charlie): make this return an HRESULT

		MappedResource<T> Get()
		{
			return MappedResource<T>(this);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Set(ID3D11DeviceContext *context, T *data)
		{
			DXR(TypelessBuffer::Set(context, (byte *)data));
			return S_OK;
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

		//////////////////////////////////////////////////////////////////////

		uint DataSize() const
		{
			return mSizeOf;
		}

		//////////////////////////////////////////////////////////////////////

		static uint SizeOf()
		{
			return sizeof(T);
		}
	};
}