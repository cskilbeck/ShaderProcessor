//////////////////////////////////////////////////////////////////////

namespace HLSL
{
	//////////////////////////////////////////////////////////////////////

	struct ConstBufferOffset
	{
		char const *name;
		size_t offset;
	};
	
	//////////////////////////////////////////////////////////////////////

	template<typename T, uint32 OffsetCount, ConstBufferOffset const Offsets[], uint32 *Buffer> struct ConstBuffer: T
	{
		//////////////////////////////////////////////////////////////////////



		//////////////////////////////////////////////////////////////////////

		void Commit(ID3D11DeviceContext *context)
		{
			context->UpdateSubresource(mConstantBuffer, 0, null, Buffer(), 0, 0);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Initialize()
		{
			if(Buffer != null)
			{
				memcpy(this, Buffer, sizeof(T));
			}
			CD3D11_BUFFER_DESC desc(sizeof(T), D3D11_BIND_CONSTANT_BUFFER);
			D3D11_SUBRESOURCE_DATA srd = { 0 };
			srd.pSysMem = (void const *)this;
			DX(gDevice->CreateBuffer(&desc, &srd, &mConstantBuffer));
		}

		//////////////////////////////////////////////////////////////////////

		uint32 SizeOfBuffer() const
		{
			return sizeof(T);
		}

		//////////////////////////////////////////////////////////////////////

		int GetOffset(char const *name) const
		{
			for(int i = 0; i < OffsetCount; ++i)
			{
				if(strcmp(Offsets[i].name, name) == 0)
				{
					return Offsets[i].offset;
				}
			}
			return -1;
		}

		//////////////////////////////////////////////////////////////////////

		void *AddressOf(char const *name) const
		{
			for(int i = 0; i < OffsetCount; ++i)
			{
				if(strcmp(Offsets[i].name, name) == 0)
				{
					return (byte *)this + Offsets[i].offset;
				}
			}
			return null;
		}

		DXPtr<ID3D11Buffer>	mConstantBuffer;
	};

	//////////////////////////////////////////////////////////////////////

	struct PixelShaderBase
	{
		uint32 SamplerCount;
		uint32 ResourceCount;
		uint32 ConstBufferCount;
		char const * const *SamplerNames;
		char const * const *ResourceNames;
		char const * const *ConstBufferNames;
		uint32 const * const ConstBufferOffsets;

		PixelShaderBase(uint32 samplerCount,
						uint32 resourceCount,
						uint32 constBufferCount,
						char const * const *samplerNames,
						char const * const *resourceNames,
						char const * const *constBufferNames,
						uint32 const * const constBufferOffsets)
			: SamplerCount(samplerCount)
			, ResourceCount(resourceCount)
			, ConstBufferCount(constBufferCount)
			, SamplerNames(samplerNames)
			, ResourceNames(resourceNames)
			, ConstBufferNames(constBufferNames)
			, ConstBufferOffsets(constBufferOffsets)
		{
		}

		template<typename T> bool GetConstBuffer(char const *name, T * &ptr)
		{
			for(uint i = 0; i < ConstBufferCount; ++i)
			{
				if(strcmp(ConstBufferNames[i], name) == 0)
				{
					ptr = (T *)((byte *)this + ConstBufferOffsets[i]);
					return true;
				}
			}
			return false;
		}

		// Same for Resources and SamplerStates

	};


} // HLSL


