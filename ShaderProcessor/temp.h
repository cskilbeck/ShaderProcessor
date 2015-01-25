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

	template<typename T, uint32 OffsetCount, ConstBufferOffset const OffsetTable[], uint32 *BufferOrDefaults> struct ConstBuffer: T
	{
		//////////////////////////////////////////////////////////////////////

		void const *Buffer() const
		{
			return BufferOrDefaults;
		}

		//////////////////////////////////////////////////////////////////////

		ConstBufferOffset const * const Offsets() const
		{
			return OffsetTable;
		}

		//////////////////////////////////////////////////////////////////////

		int GetOffset(char const *name) const
		{
			for(int i = 0; i < OffsetCount; ++i)
			{
				if(strcmp(OffsetTable[i].name, name) == 0)
				{
					return OffsetTable[i].offset;
				}
			}
			return -1;
		}

		//////////////////////////////////////////////////////////////////////

		void *GetAddressOf(char const *name) const
		{
			for(int i = 0; i < OffsetCount; ++i)
			{
				if(strcmp(OffsetTable[i].name, name) == 0)
				{
					return (byte *)this + OffsetTable[i].offset;
				}
			}
			return null;
		}
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


