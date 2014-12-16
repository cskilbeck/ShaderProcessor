
namespace HLSL
{
	struct NamePtr
	{
		char const *name;
		void *addr;
	};
	struct ConstantBuffer
	{
		virtual NamePtr const *GetNameTable() const = 0;
		virtual size_t Names() const = 0;
		virtual void *BaseAddress() const = 0;
		virtual size_t Size() const = 0;

		void *GetAddressOf(const const *name)
		{
			NamePtr const *nameTable = GetNameTable();
			for(int i = 0, l = Names(); i < l; ++i)
			{
				if(strcmp(name, nameTable[i].name) == 0)
				{
					return nameTable[i].addr;
				}
			}
			return null;
		}

	};
	struct Shader
	{
	};

	struct PixelShader_t: Shader
	{
		SamplerState *samplerState;

		Texture2D *picture;

		struct ColourModifiers_t: ConstantBuffer
		{
			#pragma pack(push, 4)
			float4 ChannelMask;
			float4 ColorOffset;
			#pragma pack(pop)

			NamePtr const *GetNameTable() const;
			size_t Names() const;
			void *BaseAddress() const;
			size_t Size() const;
		};
		ColourModifiers_t ColourModifiers;

		struct GridStuff_t: ConstantBuffer
		{
			#pragma pack(push, 4)
			float4 GridColor0;
			float4 GridColor1;
			float2 GridSize;
			float2 GridSize2;
			#pragma pack(pop)

			NamePtr const *GetNameTable() const;
			size_t Names() const;
			void *BaseAddress() const;
			size_t Size() const;
		};
		GridStuff_t GridStuff;
	};

	#if defined(DEFINE_CBUFFERS)
		const Shader::NamePtr PixelShader_ColourModifiers_NameTable[2] =
		{
			{ "ChannelMask", 0 },
			{ "ColorOffset", 16 }
		};

		const NamePtr PixelShader_GridStuff_NameTable[4] =
		{
			{ "GridColor0", 0 },
			{ "GridColor1", 16 },
			{ "GridSize", 32 },
			{ "GridSize2", 40 }
		};
	#else
		extern NamePtr PixelShader_ColourModifiers_NameTable[2];
		extern NamePtr PixelShader_GridStuff_NameTable[4];
	#endif

	inline void *PixelShader_t::ColourModifiers_t::BaseAddress() const
	{
		return &ChannelMask;
	}

	inline size_t PixelShader_t::ColourModifiers_t::Size() const
	{
		return 32;
	}

	inline size_t PixelShader_t::ColourModifiers_t::VarCount() const
	{
		return 2;
	}

	inline NamePtr *PixelShader_t::ColourModifiers_t::GetNameTable() const
	{
		return PixelShader_ColourModifiers_NameTable;
	}

	inline void *PixelShader_t::GridStuff_f::BaseAddress() const
	{
		return &GridColor0;
	}

	inline size_t PixelShader_t::GridStuff_f::Size() const
	{
		return 48;
	}

	inline size_t PixelShader_t::GridStuff_t::VarCount() const
	{
		return 4;
	}

	inline NamePtr *PixelShader_t::GridStuff_f::GetNameTable() const
	{
		return PixelShader_GridStuff_NameTable;
	}

	__declspec(selectany) PixelShader_t PixelShader =
	{
		null,
		null,
		{
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0 },
			{ 0, 0 }
		}
	};

} // HLSL