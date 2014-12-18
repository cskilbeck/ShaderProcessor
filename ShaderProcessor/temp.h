//////////////////////////////////////////////////////////////////////

namespace HLSL
{
	struct CBufferOffset
	{
		char const *name;
		uint32 offset;
	};

	template<typename T, uint32 OffsetCount, CBufferOffset const Offsets[]> struct ConstantBuffer: T
	{
		ConstantBuffer()
		{
		}

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

} // HLSL

//////////////////////////////////////////////////////////////////////

namespace HLSL
{
#pragma pack(push, 4)

	// $Globals Offsets
	DECLSPEC_SELECTANY extern CBufferOffset const SimplePixelShader_$Globals_Offsets[1] =
	{
		{ "off", 0 }
	};

	// $Globals Defaults
	DECLSPEC_SELECTANY extern uint32 const SimplePixelShader_$Globals_Defaults[1] =
	{
		0x00000000
	};

	// ColourModifiers Offsets
	DECLSPEC_SELECTANY extern CBufferOffset const SimplePixelShader_ColourModifiers_Offsets[2] =
	{
		{ "ChannelMask", 0 },
		{ "ColorOffset", 16 }
	};

	// ColourModifiers Defaults
	DECLSPEC_SELECTANY extern uint32 const SimplePixelShader_ColourModifiers_Defaults[8] =
	{
		0x89734783, 0x89734783, 0x89734783, 0x89734783,
		0x89734783, 0x89734783, 0x89734783, 0x89734783
	};

	// GridStuff Offsets
	DECLSPEC_SELECTANY extern CBufferOffset const SimplePixelShader_GridStuff_Offsets[4] =
	{
		{ "GridColor0", 0 },
		{ "GridColor1", 16 },
		{ "GridSize", 32 },
		{ "GridSize2", 40 }
	};

	// GridStuff Defaults
	DECLSPEC_SELECTANY extern uint32 const SimplePixelShader_GridStuff_Defaults[12] =
	{
		0x89734783, 0x89734783, 0x89734783, 0x89734783,
		0x89734783, 0x89734783, 0x89734783, 0x89734783,
		0x89734783, 0x89734783,
		0x89734783, 0x89734783
	};

	// SimplePixelShader
	struct SimplePixelShader_t : PixelShader
	{
		// $Globals
		struct $Globals_t
		{
			Float4 off;
		};
		ConstantBuffer <$Globals_t, 1, SimplePixelShader_$Globals_Offsets> $Globals;

		// ColourModifiers
		struct ColourModifiers_t
		{
			Float4 ChannelMask;
			Float4 ColorOffset;
		};
		ConstantBuffer<ColourModifiers_t, 4, SimplePixelShader_ColourModifiers_Offsets> ColourModifiers;

		// GridStuff
		struct GridStuff_t
		{
			Float4 GridColor0;
			Float4 GridColor1;
			Float2 GridSize;
			Float2 GridSize2;
		};
		ConstantBuffer<GridStuff_t, 2, SimplePixelShader_GridStuff_Offsets> GridStuff;

		// Samplers
		SamplerState *samplerState;

		// Textures
		Texture2D *picture;

		// Constructor
		SimplePixelShader_t()
			: $Globals(SimplePixelShader_$Globals_Defaults)
			, ColourModifiers(SimplePixelShader_ColourModifiers_Defaults)
			, GridStuff(SimplePixelShader_GridStuff_Defaults)
			, samplerState(null)
			, picture(null)
		{
		}
	};

	DECLSPEC_SELECTANY SimplePixelShader_t SimplePixelShader;

#pragma pack(pop)

} // HLSL



