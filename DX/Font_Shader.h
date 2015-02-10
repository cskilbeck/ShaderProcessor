//////////////////////////////////////////////////////////////////////
// Font.h - auto generated file, do not edit

#pragma once
#pragma pack(push, 4)

namespace Shaders
{
	using namespace DX;

	namespace Font
	{
		//////////////////////////////////////////////////////////////////////
		// Data for VS

		uint32 WEAKSYM VS_Data[] =
		{
			0x43425844,0xa60156a6,0x735a32e7,0xf739f496,0x630d3301,0x00000001,0x00000358,0x00000005,
			0x00000034,0x00000108,0x00000168,0x000001c0,0x000002dc,0x46454452,0x000000cc,0x00000001,
			0x00000048,0x00000001,0x0000001c,0xfffe0400,0x00000100,0x00000098,0x0000003c,0x00000000,
			0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x6e6f4376,0x6e617473,
			0xab007374,0x0000003c,0x00000001,0x00000060,0x00000040,0x00000000,0x00000000,0x00000078,
			0x00000000,0x00000040,0x00000002,0x00000088,0x00000000,0x6e617254,0x726f6673,0x74614d6d,
			0x00786972,0x00030003,0x00040004,0x00000000,0x00000000,0x7263694d,0x666f736f,0x52282074,
			0x4c482029,0x53204c53,0x65646168,0x6f432072,0x6c69706d,0x36207265,0x392e332e,0x2e303036,
			0x38333631,0xabab0034,0x4e475349,0x00000058,0x00000002,0x00000008,0x00000038,0x00000000,
			0x00000000,0x00000003,0x00000000,0x00000307,0x00000047,0x00000000,0x00000000,0x00000003,
			0x00000001,0x00000303,0x616f6c66,0x6f505f74,0x69746973,0x68006e6f,0x5f666c61,0x43786554,
			0x64726f6f,0xababab00,0x4e47534f,0x00000050,0x00000002,0x00000008,0x00000038,0x00000000,
			0x00000001,0x00000003,0x00000000,0x0000000f,0x00000044,0x00000002,0x00000000,0x00000003,
			0x00000001,0x00000c03,0x505f5653,0x7469736f,0x006e6f69,0x43584554,0x44524f4f,0xababab00,
			0x52444853,0x00000114,0x00010040,0x00000045,0x04000059,0x00208e46,0x00000000,0x00000004,
			0x0300005f,0x00101032,0x00000000,0x0300005f,0x00101032,0x00000001,0x04000067,0x001020f2,
			0x00000000,0x00000001,0x03000065,0x00102032,0x00000001,0x02000068,0x00000001,0x05000036,
			0x00100032,0x00000000,0x00101046,0x00000000,0x05000036,0x00100042,0x00000000,0x00004001,
			0x3f800000,0x08000010,0x00102012,0x00000000,0x00100246,0x00000000,0x00208346,0x00000000,
			0x00000000,0x08000010,0x00102022,0x00000000,0x00100246,0x00000000,0x00208346,0x00000000,
			0x00000001,0x08000010,0x00102042,0x00000000,0x00100246,0x00000000,0x00208346,0x00000000,
			0x00000002,0x08000010,0x00102082,0x00000000,0x00100246,0x00000000,0x00208346,0x00000000,
			0x00000003,0x05000036,0x00102032,0x00000001,0x00101046,0x00000001,0x0100003e,0x54415453,
			0x00000074,0x00000008,0x00000001,0x00000000,0x00000004,0x00000004,0x00000000,0x00000000,
			0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
			0x00000000,0x00000000,0x00000000,0x00000000,0x00000003,0x00000000,0x00000000,0x00000000,
			0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
		};

		//////////////////////////////////////////////////////////////////////
		// offsets and defaults

		//////////////////////////////////////////////////////////////////////
		// vConstants offsets

		extern ConstBufferOffset const WEAKSYM VS_vConstants_Offsets[1] =
		{
			{ "TransformMatrix", 0 }
		};

		// no defaults for vConstants


		//////////////////////////////////////////////////////////////////////
		// const buffer names table

		extern char const WEAKSYM *VS_ConstBufferNames[] =
		{
			"vConstants"
		};

		//////////////////////////////////////////////////////////////////////
		// Input Element descs

		extern D3D11_INPUT_ELEMENT_DESC const WEAKSYM VS_InputElements[2] =
		{
			{ "float_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "half_TexCoord", 0, DXGI_FORMAT_R16G16_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		//////////////////////////////////////////////////////////////////////
		// InputVertex

		struct InputVertex
		{
			Float3 Position;
			Half2 TexCoord;
		};

		using VertBuffer = VertexBuffer<InputVertex>;

		//////////////////////////////////////////////////////////////////////
		// Vertex Shader: VS

		struct VS : VertexShader, Aligned16
		{
			// Const Buffers

			struct vConstants_t: Aligned16
			{
				Float4x4 TransformMatrix;
			};

			ConstBuffer<vConstants_t> vConstants;

			// Constructor

			VS()
				: VertexShader(VS_Data, 856, 1, VS_ConstBufferNames, 0, null, 0, null, null, null, VS_InputElements, _countof(VS_InputElements))
				, vConstants(1, VS_vConstants_Offsets, null, this, 0)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Data for PS

		uint32 WEAKSYM PS_Data[] =
		{
			0x43425844,0x3de573c1,0x45b29681,0x89f2f6db,0x6b925e56,0x00000001,0x000002f8,0x00000005,
			0x00000034,0x0000014c,0x000001a4,0x000001d8,0x0000027c,0x46454452,0x00000110,0x00000001,
			0x00000094,0x00000003,0x0000001c,0xffff0400,0x00000100,0x000000dc,0x0000007c,0x00000003,
			0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x00000082,0x00000002,
			0x00000005,0x00000004,0xffffffff,0x00000000,0x00000001,0x0000000c,0x00000087,0x00000000,
			0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x6c706d73,0x61700072,
			0x70006567,0x736e6f43,0x746e6174,0xabab0073,0x00000087,0x00000001,0x000000ac,0x00000010,
			0x00000000,0x00000000,0x000000c4,0x00000000,0x00000010,0x00000002,0x000000cc,0x00000000,
			0x6f6c6f43,0xabab0072,0x00030001,0x00040001,0x00000000,0x00000000,0x7263694d,0x666f736f,
			0x52282074,0x4c482029,0x53204c53,0x65646168,0x6f432072,0x6c69706d,0x36207265,0x392e332e,
			0x2e303036,0x38333631,0xabab0034,0x4e475349,0x00000050,0x00000002,0x00000008,0x00000038,
			0x00000000,0x00000001,0x00000003,0x00000000,0x0000000f,0x00000044,0x00000002,0x00000000,
			0x00000003,0x00000001,0x00000303,0x505f5653,0x7469736f,0x006e6f69,0x43584554,0x44524f4f,
			0xababab00,0x4e47534f,0x0000002c,0x00000001,0x00000008,0x00000020,0x00000000,0x00000000,
			0x00000003,0x00000000,0x0000000f,0x545f5653,0x45475241,0xabab0054,0x52444853,0x0000009c,
			0x00000040,0x00000027,0x04000059,0x00208e46,0x00000000,0x00000001,0x0300005a,0x00106000,
			0x00000000,0x04001858,0x00107000,0x00000000,0x00005555,0x03001062,0x00101032,0x00000001,
			0x03000065,0x001020f2,0x00000000,0x02000068,0x00000001,0x09000045,0x001000f2,0x00000000,
			0x00101046,0x00000001,0x00107e46,0x00000000,0x00106000,0x00000000,0x08000038,0x001020f2,
			0x00000000,0x00100e46,0x00000000,0x00208e46,0x00000000,0x00000000,0x0100003e,0x54415453,
			0x00000074,0x00000003,0x00000001,0x00000000,0x00000002,0x00000001,0x00000000,0x00000000,
			0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,
			0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
			0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
		};

		//////////////////////////////////////////////////////////////////////
		// offsets and defaults

		//////////////////////////////////////////////////////////////////////
		// pConstants offsets

		extern ConstBufferOffset const WEAKSYM PS_pConstants_Offsets[1] =
		{
			{ "Color", 0 }
		};

		// no defaults for pConstants


		//////////////////////////////////////////////////////////////////////
		// const buffer names table

		extern char const WEAKSYM *PS_ConstBufferNames[] =
		{
			"pConstants"
		};

		//////////////////////////////////////////////////////////////////////
		// Sampler names

		extern char const WEAKSYM * PS_SamplerNames[] =
		{
			"smplr"
		};

		//////////////////////////////////////////////////////////////////////
		// Texture names

		extern char const WEAKSYM * PS_TextureNames[] =
		{
			"page"
		};

		//////////////////////////////////////////////////////////////////////
		// Pixel Shader: PS

		struct PS : PixelShader, Aligned16
		{
			// Const Buffers

			struct pConstants_t: Aligned16
			{
				Float4 Color;
			};

			ConstBuffer<pConstants_t> pConstants;

			// Samplers

			union
			{
				struct
				{
					Sampler *smplr;
				};
				Sampler *samplers[1];
			};

			// Textures

			union
			{
				struct
				{
					Texture *page;
				};
				Texture *textures[1];
			};

			// Constructor

			PS()
				: PixelShader(PS_Data, 760, 1, PS_ConstBufferNames, 1, PS_SamplerNames, 1, PS_TextureNames, textures, samplers)
				, smplr(null)
				, page(null)
				, pConstants(1, PS_pConstants_Offsets, null, this, 0)
			{
			}
		};

	} // Font

} // Shaders

#pragma pack(pop)