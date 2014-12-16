//////////////////////////////////////////////////////////////////////

namespace HLSL
{
	struct CBufferOffset
	{
		char const *name;
		uint32 offset;
	};

	template<typename T, uint32 OffsetCount, CBufferOffset const Offsets[]> struct ConstantBuffer : T
	{
		ConstantBuffer(uint32 const *defaults)
		{
			memcpy(this, defaults, sizeof(T));	// !
		}

		template<typename T> T &GetAddressOf(char const *name)
		{
			for(int i = 0; i < OffsetCount; ++i)
			{
				if(strcmp(name, Offsets[i].name) == 0)
				{
					return (T &)(*((byte *)this + Offsets[i].offset));
				}
			}
			return (T &)null;
		}
	};

	struct Shader
	{
		// blah blah
	};

	struct SamplerState;
	struct Texture2D;

} // HLSL

//////////////////////////////////////////////////////////////////////

namespace HLSL
{
	#if defined(DEFINE_CBUFFERS)

		extern CBufferOffset const PixelShader_ColourModifiers_OffsetTable[2] =
		{
			{ "ChannelMask", 0 },
			{ "ColorOffset", 16 }
		};

		extern CBufferOffset const PixelShader_GridStuff_OffsetTable[4] =
		{
			{ "GridColor0", 0 },
			{ "GridColor1", 16 },
			{ "GridSize", 32 },
			{ "GridSize2", 40 }
		};

		uint32 const PixelShader_ColourModifiers_ValuesTable[8] =
		{
			0x89734783, 0x89734783, 0x89734783, 0x89734783,
			0x89734783, 0x89734783, 0x89734783, 0x89734783
		};

		uint32 const PixelShader_GridStuff_ValuesTable[12] =
		{
			0x89734783, 0x89734783, 0x89734783, 0x89734783,
			0x89734783, 0x89734783, 0x89734783, 0x89734783,
			0x89734783, 0x89734783,
			0x89734783, 0x89734783
		};

	#else

		extern CBufferOffset const PixelShader_ColourModifiers_OffsetTable[2];
		extern CBufferOffset const PixelShader_GridStuff_OffsetTable[4];
		extern uint32 const PixelShader_ColourModifiers_ValuesTable[8];
		extern uint32 const PixelShader_GridStuff_ValuesTable[12];

	#endif

	#pragma pack(push, 4)

	struct PixelShader_t
	{
		struct ColourModifiers_t
		{
			Float4 ChannelMask;
			Float4 ColorOffset;
		};

		struct GridStuff_t
		{
			Float4 GridColor0;
			Float4 GridColor1;
			Float2 GridSize;
			Float2 GridSize2;
		};

		PixelShader_t(SamplerState *s = null, Texture2D *t = null) :
			samplerState(s),
			picture(t),
			ColourModifiers(PixelShader_ColourModifiers_ValuesTable),
			GridStuff(PixelShader_GridStuff_ValuesTable)
		{
		}

		using GridStuff_s = ConstantBuffer<GridStuff_t, 2, PixelShader_GridStuff_OffsetTable>;
		using ColourModifiers_s = ConstantBuffer<ColourModifiers_t, 4, PixelShader_ColourModifiers_OffsetTable>;

		SamplerState *samplerState;
		Texture2D *picture;
		ColourModifiers_s ColourModifiers;
		GridStuff_s GridStuff;
	};

	#pragma pack(pop)

	#if defined(DEFINE_CBUFFERS)
		PixelShader_t PixelShader;
	#else
		extern PixelShader_t PixelShader;
	#endif

} // HLSL