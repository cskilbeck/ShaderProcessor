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

	template<typename definition> struct ConstBuffer: definition, Buffer<definition>	// definition MUST be POD and Aligned16
	{
		ConstBuffer()
			: Buffer<definition>()
			, mIndex(0)
			, mOffsetCount(0)
			, mOffsets(null)
			, mDefaults(null)
		{
		}

		HRESULT Create(uint32 OffsetCount, ConstBufferOffset const Offsets[], uint32 *Defaults, Shader *parent, uint index, uint bindPoint)
		{
			mOffsetCount = OffsetCount;
			mOffsets = Offsets;
			mDefaults = Defaults;
			mIndex = index;
			if(mDefaults != null)
			{
				memcpy(this, mDefaults, sizeof(definition)); // definition must be most-derived and there can be no virtuals!
			}
			else
			{
				memset(this, 0, sizeof(definition));
			}
			DXR(CreateBuffer(BufferType::ConstantBufferType, 1, this, DynamicUsage, Writeable));
			parent->AddConstBuffer(this, bindPoint);
			assert(parent->mConstBuffers.size() <= parent->mNumConstBuffers);
			return S_OK;
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

		HRESULT Update(ID3D11DeviceContext *context)
		{
			definition *d;
			DXR(Map(context, d));
			memcpy(d, (definition *)this, sizeof(definition));
			UnMap(context);
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Update()
		{
			definition *d;
			DXR(Map(DX::Context, d));
			memcpy(d, (definition *)this, sizeof(definition));
			UnMap(DX::Context);
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		uint32 Index() const
		{
			return mIndex;
		}

		//////////////////////////////////////////////////////////////////////

	private:

		uint32							mIndex;
		uint32							mOffsetCount;
		ConstBufferOffset const *		mOffsets;
		uint32 *			 			mDefaults;
	};

}