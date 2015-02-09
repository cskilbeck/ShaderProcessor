//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	template <typename T> struct Buffer
	{
		//////////////////////////////////////////////////////////////////////

		Buffer()
			: mOwnData(false)
		{
		}

		//////////////////////////////////////////////////////////////////////

		~Buffer()
		{
			if(mOwnData)
			{
				Delete(mData);
			}
		}

		//////////////////////////////////////////////////////////////////////

		Buffer(BufferType type, uint count, T *data = null, BufferUsage usage = DefaultUsage, ReadWriteOption rwOption = NotCPUAccessible)
		{
			Create(type, count, data, usage, rwOption);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(BufferType type, uint count, T *data = null, BufferUsage usage = DefaultUsage, ReadWriteOption rwOption = NotCPUAccessible)
		{
			mCount = count;
			mData = data;
			if(mData == null)
			{
				mData = new T[count];
				mOwnData = true;
			}
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
			D3D11_SUBRESOURCE_DATA srd = { mData, 0, 0 };
			DXR(DX::Device->CreateBuffer(&desc, &srd, &mBuffer));
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
			mOwnData = false;
			Commit();
		}

		//////////////////////////////////////////////////////////////////////

		T &operator[](uint index)
		{
			return mData[index];
		}

		//////////////////////////////////////////////////////////////////////

		T *Data()
		{
			return mData;
		}

		//////////////////////////////////////////////////////////////////////

		void Commit(ID3D11DeviceContext *context)
		{
			context->UpdateSubresource(mBuffer, 0, null, mData, 0, 0);
		}

		//////////////////////////////////////////////////////////////////////

		uint32 Count() const
		{
			return mCount;
		}

		//////////////////////////////////////////////////////////////////////

	protected:

		T *mData;
		bool mOwnData;
		uint32 mCount;
		DXPtr<ID3D11Buffer> mBuffer;
		BufferUsage mUsage;
		ReadWriteOption mRWOption;
		D3D11_MAP mMapType;
	};

}