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

		TypelessBuffer(BufferType type, uint32 size, byte *data = null, BufferUsage usage = DynamicUsage, ReadWriteOption rwOption = Writeable)
		{
			Create(type, size, data, usage, rwOption);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(BufferType type, uint32 size, byte *data = null, BufferUsage usage = DynamicUsage, ReadWriteOption rwOption = Writeable)
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

		void UnMap()
		{
			DX::Context->Unmap(mBuffer, 0);
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
			void *p = Map(context);
			memcpy(p, data, mSize);
			UnMap(context);
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

		void Commit(ID3D11DeviceContext *context)
		{
			void *p = Map(context);
			assert(p != null);
			memcpy(p, mData, mSize);
			UnMap(context);
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

	struct BoundBuffer : TypelessBuffer
	{
		uint mBindPoint;

		BoundBuffer(uint bindPoint)
			: mBindPoint(bindPoint)
		{
		}
	};

	//////////////////////////////////////////////////////////////////////

	template <typename T, typename U> struct Buffer: U
	{
		//////////////////////////////////////////////////////////////////////

		template <typename T, typename U> struct MappedResource
		{
			Buffer<T, U> *buffer;
			T * resource;

			MappedResource(Buffer<T, U> *buf)
				: buffer(buf)
			{
				DXI(resource = buffer->Map());
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

		Buffer(uint bindPoint)
			: U(bindPoint)
		{
		}

		//////////////////////////////////////////////////////////////////////

		Buffer()
			: U()
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
			return U::Create(type, count * sizeof(T), (byte *)data, usage, rwOption);
		}

		//////////////////////////////////////////////////////////////////////

		T * const Map(ID3D11DeviceContext *context, MapWaitOption waitOption = WaitForGPU)
		{
			return (T * const)TypelessBuffer::Map(context, waitOption);
		}

		//////////////////////////////////////////////////////////////////////

		T * const Map(MapWaitOption waitOption = WaitForGPU)
		{
			return (T * const)TypelessBuffer::Map(DX::Context, waitOption);
		}

		//////////////////////////////////////////////////////////////////////

		MappedResource<T, U> Get()
		{
			return MappedResource<T, U>(this);
		}

		//////////////////////////////////////////////////////////////////////

		void Set(ID3D11DeviceContext *context, T *data)
		{
			U::Set(content, (byte *)data);
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
			return (T *)U::Data();
		}
	};

}