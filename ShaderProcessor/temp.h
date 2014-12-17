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

	struct VertexShader
	{
	};

	struct PixelShader
	{
	};

	struct SamplerState : Reportable
	{
		void StaticsOutput(D3D11_SHADER_INPUT_BIND_DESC desc) override
		{
		}

		void MemberOutput(D3D11_SHADER_INPUT_BIND_DESC desc) override
		{

		}

		void ConstructorOutput(D3D11_SHADER_INPUT_BIND_DESC desc) override
		{

		}
	};

	struct Texture2D : Reportable
	{
		void StaticsOutput(D3D11_SHADER_INPUT_BIND_DESC desc) override
		{
		}

		void MemberOutput(D3D11_SHADER_INPUT_BIND_DESC desc) override
		{
		}

		void ConstructorOutput(D3D11_SHADER_INPUT_BIND_DESC desc) override
		{
		}
	};

} // HLSL

//////////////////////////////////////////////////////////////////////

namespace HLSL
{
#pragma pack(push, 4)

	// $Globals Offsets
	DECLSPEC_SELECTANY extern CBufferOffset const SimplePixelShader_$Globals_OffsetTable[1] =
	{
		{ "off", 0 }
	};

	// $Globals Defaults
	DECLSPEC_SELECTANY extern uint32 const SimplePixelShader_$Globals_DefaultsTable[1] =
	{
		0x00000000
	};

	// ColourModifiers Offsets
	DECLSPEC_SELECTANY extern CBufferOffset const SimplePixelShader_ColourModifiers_OffsetTable[2] =
	{
		{ "ChannelMask", 0 },
		{ "ColorOffset", 16 }
	};

	// ColourModifiers Defaults
	DECLSPEC_SELECTANY extern uint32 const SimplePixelShader_ColourModifiers_DefaultsTable[8] =
	{
		0x89734783, 0x89734783, 0x89734783, 0x89734783,
		0x89734783, 0x89734783, 0x89734783, 0x89734783
	};

	// GridStuff Offsets
	DECLSPEC_SELECTANY extern CBufferOffset const SimplePixelShader_GridStuff_OffsetTable[4] =
	{
		{ "GridColor0", 0 },
		{ "GridColor1", 16 },
		{ "GridSize", 32 },
		{ "GridSize2", 40 }
	};

	// GridStuff Defaults
	DECLSPEC_SELECTANY extern uint32 const SimplePixelShader_GridStuff_DefaultsTable[12] =
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
		ConstantBuffer <$Globals_t, 1, SimplePixelShader_$Globals_OffsetTable> $Globals;

		// ColourModifiers
		struct ColourModifiers_t
		{
			Float4 ChannelMask;
			Float4 ColorOffset;
		};
		ConstantBuffer<ColourModifiers_t, 4, SimplePixelShader_ColourModifiers_OffsetTable> ColourModifiers;

		// GridStuff
		struct GridStuff_t
		{
			Float4 GridColor0;
			Float4 GridColor1;
			Float2 GridSize;
			Float2 GridSize2;
		};
		ConstantBuffer<GridStuff_t, 2, SimplePixelShader_GridStuff_OffsetTable> GridStuff;

		// Samplers
		SamplerState *samplerState;

		// Textures
		Texture2D *picture;

		// Constructor
		SimplePixelShader_t()
			: $Globals(SimplePixelShader_$Globals_DefaultsTable)
			, ColourModifiers(SimplePixelShader_ColourModifiers_DefaultsTable)
			, GridStuff(SimplePixelShader_GridStuff_DefaultsTable)
			, samplerState(null)
			, picture(null)
		{
		}
	};

	DECLSPEC_SELECTANY SimplePixelShader_t SimplePixelShader;

#pragma pack(pop)

} // HLSL