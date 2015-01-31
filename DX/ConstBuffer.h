//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct ConstBufferOffset
{
	char const *name;
	size_t offset;
};

template<typename definition> struct ALIGNED(16) ConstBuffer : definition	// definition MUST be POD
{
	//////////////////////////////////////////////////////////////////////

	uint32 const					mOffsetCount;
	ConstBufferOffset const * const	mOffsets;
	uint32 const * const 			mDefaults;
	DXPtr<ID3D11Buffer>				mConstantBuffer;

	ConstBuffer(uint32 OffsetCount, ConstBufferOffset const Offsets[], uint32 const *Defaults, Shader *parent)
		: mOffsetCount(OffsetCount)
		, mOffsets(Offsets)
		, mDefaults(Defaults)
	{
		parent->mConstBuffers.push_back(this);
		ResetToDefaults();
		CD3D11_BUFFER_DESC desc(sizeof(definition), D3D11_BIND_CONSTANT_BUFFER);
		D3D11_SUBRESOURCE_DATA srd = { 0 };
		srd.pSysMem = (void const *)this;
		DXV(gDevice->CreateBuffer(&desc, &srd, &mConstantBuffer));
		parent->mConstBufferPointers.push_back(mConstantBuffer);
		assert(parent->mConstBuffers.size() <= parent->mNumConstBuffers);
	}

	//////////////////////////////////////////////////////////////////////

	void Commit(ID3D11DeviceContext *context)
	{
		context->UpdateSubresource(mConstantBuffer, 0, null, this, 0, 0);
	}

	//////////////////////////////////////////////////////////////////////

	void ResetToDefaults()
	{
		if(mDefaults != null)
		{
			memcpy(this, mDefaults, sizeof(definition)); // !!
		}
		else
		{
			memset(this, 0, sizeof(definition));
		}
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

	void *AddressOf(char const *name) const
	{
		int offset;
		return GetOffset(name, offset) ? (byte *)this + offset : null;
	}
};

