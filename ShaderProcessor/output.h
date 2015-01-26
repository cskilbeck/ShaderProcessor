namespace HLSL
{
#pragma pack(push, KNOB, 4)

	// $Globals Offsets
	extern ConstBufferOffset const WEAKSYM SimplePixelShader_$Globals_Offsets[2] =
	{
		{ "bill", 0 },
		{ "off", 16 }
	};

	// ColourModifiers Offsets
	extern ConstBufferOffset const WEAKSYM SimplePixelShader_ColourModifiers_Offsets[2] =
	{
		{ "ChannelMask", 0 },
		{ "ColorOffset", 16 }
	};

	// ColourModifiers Defaults
	extern uint32 ALIGNED(16) WEAKSYM SimplePixelShader_ColourModifiers_Defaults[8] =
	{
		0x3f800000,0x3f800000,0x3f800000,0x3f800000, // ChannelMask
		0x00000000,0x00000000,0x00000000,0x00000000  // ColorOffset
	};

	// GridStuff Offsets
	extern ConstBufferOffset const WEAKSYM SimplePixelShader_GridStuff_Offsets[4] =
	{
		{ "GridColor0", 0 },
		{ "GridSize", 16 },
		{ "GridColor1", 32 },
		{ "GridSize2", 48 }
	};

	// GridStuff Defaults
	extern uint32 ALIGNED(16) WEAKSYM SimplePixelShader_GridStuff_Defaults[16] =
	{
		0x3f4ccccd,0x3f4ccccd,0x3f4ccccd,0x3f800000, // GridColor0
		0x00000000,0x00000000,0x00000000,0x00000000, // GridSize
		0x3f19999a,0x3f19999a,0x3f19999a,0x3f800000, // GridColor1
		0x42000000,0x42000000,0x00000000,0x00000000  // GridSize2
	};

	// SetSamplers
	extern char const WEAKSYM * const SimplePixelShader_Sampler_Names[1] =
	{
		"samplerState"
	};

	// SetShaderResourceView
	extern char const WEAKSYM * const SimplePixelShader_Resource_Names[1] =
	{
		"picture"
	};

	// SetConstantBuffers
	extern char const WEAKSYM * const SimplePixelShader_ConstBuffer_Names[3] =
	{
		"$Globals",
		"ColourModifiers",
		"GridStuff"
	};

	struct ShaderResource;

	struct SimplePixelShader_t : PixelShaderBase
	{
		struct ALIGNED(16) $Globals_t
		{
			Float2 bill;			byte pad0[8];
			Float4 off;
		};

		ConstBuffer<$Globals_t,
					2,
					SimplePixelShader_$Globals_Offsets,
					null>
				$Globals;

		struct ALIGNED(16) ColourModifiers_t
		{
			Float4 ChannelMask;
			Float4 ColorOffset;
		};

		ConstBuffer<ColourModifiers_t,
					2,
					SimplePixelShader_ColourModifiers_Offsets,
					SimplePixelShader_ColourModifiers_Defaults>
				ColourModifiers;

		struct ALIGNED(16) GridStuff_t
		{
			Float4 GridColor0;
			Float2 GridSize;			byte pad0[8];
			Float4 GridColor1;
			Float2 GridSize2;			byte pad1[8];
		};

		ConstBuffer<GridStuff_t,
					4,
					SimplePixelShader_GridStuff_Offsets,
					SimplePixelShader_GridStuff_Defaults>
				GridStuff;

		void *ConstantBuffers[3];

		union
		{
			struct
			{
				SamplerState *samplerState;
			};
			SamplerState *SamplerStates[1];
		};

		union
		{
			struct
			{
				Texture2D *picture;
			};
			ShaderResource *Resources[1];
		};


		// Constructor
		SimplePixelShader_t(
			uint32 samplerCount,
			uint32 resourceCount,
			uint32 constBufferCount,
			char const * const *samplerNames,
			char const * const *resourceNames,
			char const * const *constBufferNames,
			uint32 const * const constBufferOffsets)
			: PixelShaderBase(
				samplerCount,
				resourceCount,
				constBufferCount,
				samplerNames,
				resourceNames,
				constBufferNames,
				constBufferOffsets)
			, samplerState(null)
			, picture(null)
		{
			ConstantBuffers[0] = &$Globals;
			ConstantBuffers[1] = &ColourModifiers;
			ConstantBuffers[2] = &GridStuff;
		}

		size_t SamplerOffset(char const *name)
		{
			return offsetof(SimplePixelShader_t, samplerState);
		}
	};

	extern uint32 const WEAKSYM SimplePixelShader_ConstBuffer_Offsets[3] =
	{
		offsetof(SimplePixelShader_t, $Globals),
		offsetof(SimplePixelShader_t, ColourModifiers),
		offsetof(SimplePixelShader_t, GridStuff)
	};

	SimplePixelShader_t WEAKSYM SimplePixelShader(
		1,
		1,
		3,
		SimplePixelShader_Sampler_Names,
		SimplePixelShader_Resource_Names,
		SimplePixelShader_ConstBuffer_Names,
		SimplePixelShader_ConstBuffer_Offsets);

#pragma pack(pop, KNOB)

} // HLSL
