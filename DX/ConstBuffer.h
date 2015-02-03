//////////////////////////////////////////////////////////////////////

#pragma once

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
		return Buffer<T>::Create(ConstantBufferType, count, data, usage, rwOption);
	}
};

//////////////////////////////////////////////////////////////////////

template<typename definition> struct ALIGNED(16) ConstBuffer : definition, ConstantBuffer<definition>	// definition MUST be POD
{
	ConstBuffer(uint32 OffsetCount, ConstBufferOffset const Offsets[], uint32 *Defaults, Shader *parent)
		: mOffsetCount(OffsetCount)
		, mOffsets(Offsets)
		, mDefaults(Defaults)
	{
		if(mDefaults != null)
		{
			memcpy(this, mDefaults, sizeof(definition)); // !!
		}
		else
		{
			memset(this, 0, sizeof(definition));
		}
		Create(1, (definition *)this);
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

	uint32 const					mOffsetCount;
	ConstBufferOffset const * const	mOffsets;
	uint32 const * const 			mDefaults;

};

