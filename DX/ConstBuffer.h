//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct ConstBufferOffset
	{
		char const *name;
		size_t offset;
	};

	//////////////////////////////////////////////////////////////////////

	template <typename T> struct ConstantBuffer: Buffer<T>
	{
		HRESULT Create(uint count, T *data = null, BufferUsage usage = DefaultUsage, ReadWriteOption rwOption = NotCPUAccessible)
		{
			DXR(Buffer<T>::Create(ConstantBufferType, count, data, usage, rwOption));
			return S_OK;
		}

	};

	//////////////////////////////////////////////////////////////////////

	template<typename definition> struct ConstBuffer : definition, ConstantBuffer <definition>	// definition MUST be POD
	{
		ConstBuffer(uint32 OffsetCount, ConstBufferOffset const Offsets[], uint32 *Defaults, Shader *parent, uint index)
			: mOffsetCount(OffsetCount)
			, mOffsets(Offsets)
			, mDefaults(Defaults)
			, mIndex(index)
		{
			if(mDefaults != null)
			{
				memcpy(this, mDefaults, sizeof(definition));
			}
			else
			{
				memset(this, 0, sizeof(definition));
			}
			DXI(ConstantBuffer<definition>::Create(1, this, DynamicUsage, Writeable));
			parent->mConstBuffers.push_back(this);
			parent->mConstBufferPointers.push_back(mBuffer);
			assert(parent->mConstBuffers.size() <= parent->mNumConstBuffers);
		}

		//////////////////////////////////////////////////////////////////////

		bool GetOffset(char const *name, int &offset) const
		{
			for(int i = 0; i < mOffsetCount; ++i)
			{
				if(strcmp(mOffsets[i].name, name) == 0)
				{
					offset = mOffsets[i].offset;
					return true;
				}
			}
			return false;
		}

		//////////////////////////////////////////////////////////////////////

		void Update(ID3D11DeviceContext *context)
		{
			definition *d = Map(context);
			memcpy(d, (definition *)this, sizeof(definition));
			UnMap(context);
		}

		//////////////////////////////////////////////////////////////////////

		void Update()
		{
			definition *d = Map(DX::Context);
			memcpy(d, (definition *)this, sizeof(definition));
			UnMap(DX::Context);
		}

		//////////////////////////////////////////////////////////////////////

		uint32 Index() const
		{
			return mIndex;
		}

		//////////////////////////////////////////////////////////////////////

	private:

		uint32 const					mIndex;
		uint32 const					mOffsetCount;
		ConstBufferOffset const * const	mOffsets;
		uint32 const * const 			mDefaults;
	};

}