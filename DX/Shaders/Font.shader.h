//////////////////////////////////////////////////////////////////////
// Font.h - auto generated file, do not edit

#pragma once
#pragma pack(push, 4)

namespace DXShaders
{
	using namespace DX;

	// D3D11_DEPTH_STENCIL_DESC
	uint32 WEAKSYM Font_DepthStencilDesc[] = 
	{
		0x00000000,0x00000000,0x00000002,0x00000000,0x0000ffff,0x00000001,0x00000001,0x00000001,
		0x00000008,0x00000001,0x00000001,0x00000001,0x00000008
	};

	// D3D11_RASTERIZER_DESC
	uint32 WEAKSYM Font_RasterizerDesc[] = 
	{
		0x00000003,0x00000003,0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,
		0x00000000,0x00000000
	};

	// D3D11_BLEND_DESC
	uint32 WEAKSYM Font_BlendDesc[] = 
	{
		0x00000000,0x00000000,0x00000001,0x00000005,0x00000006,0x00000001,0x00000002,0x00000001,
		0x00000001,0x010ee10f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0x010ee10f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0x010ee10f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0x010ee10f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0x010ee10f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0x010ee10f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0x010ee10f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0x010ee10f
	};

	//////////////////////////////////////////////////////////////////////
	// Data for Font_VS

	uint32 WEAKSYM Font_VS_Data[] =
	{
		0x43425844,0xf4128e26,0x85eeb3f0,0xc401f596,0x54409b48,0x00000001,0x00000378,0x00000005,
		0x00000034,0x0000008c,0x0000014c,0x0000020c,0x000002fc,0x46454452,0x00000050,0x00000000,
		0x00000000,0x00000000,0x0000001c,0xfffe0400,0x00008100,0x0000001c,0x7263694d,0x666f736f,
		0x52282074,0x4c482029,0x53204c53,0x65646168,0x6f432072,0x6c69706d,0x36207265,0x392e332e,
		0x2e303036,0x31343731,0xabab0035,0x4e475349,0x000000b8,0x00000005,0x00000008,0x00000080,
		0x00000000,0x00000000,0x00000003,0x00000000,0x00000303,0x0000008f,0x00000000,0x00000000,
		0x00000003,0x00000001,0x00000303,0x00000099,0x00000000,0x00000000,0x00000003,0x00000002,
		0x00000303,0x000000a2,0x00000000,0x00000000,0x00000003,0x00000003,0x00000303,0x000000ab,
		0x00000000,0x00000000,0x00000003,0x00000004,0x00000f0f,0x616f6c66,0x6f505f74,0x69746973,
		0x68006e6f,0x5f666c61,0x657a6953,0x6c616800,0x56555f66,0x61680061,0x555f666c,0x62006256,
		0x5f657479,0x6f6c6f43,0xabab0072,0x4e47534f,0x000000b8,0x00000005,0x00000008,0x00000080,
		0x00000000,0x00000000,0x00000003,0x00000000,0x00000c03,0x0000008f,0x00000000,0x00000000,
		0x00000003,0x00000000,0x0000030c,0x00000099,0x00000000,0x00000000,0x00000003,0x00000001,
		0x00000c03,0x000000a2,0x00000000,0x00000000,0x00000003,0x00000001,0x0000030c,0x000000ab,
		0x00000000,0x00000000,0x00000003,0x00000002,0x0000000f,0x616f6c66,0x6f505f74,0x69746973,
		0x68006e6f,0x5f666c61,0x657a6953,0x6c616800,0x56555f66,0x61680061,0x555f666c,0x62006256,
		0x5f657479,0x6f6c6f43,0xabab0072,0x52444853,0x000000e8,0x00010040,0x0000003a,0x0300005f,
		0x00101032,0x00000000,0x0300005f,0x00101032,0x00000001,0x0300005f,0x00101032,0x00000002,
		0x0300005f,0x00101032,0x00000003,0x0300005f,0x001010f2,0x00000004,0x03000065,0x00102032,
		0x00000000,0x03000065,0x001020c2,0x00000000,0x03000065,0x00102032,0x00000001,0x03000065,
		0x001020c2,0x00000001,0x03000065,0x001020f2,0x00000002,0x05000036,0x00102032,0x00000000,
		0x00101046,0x00000000,0x05000036,0x001020c2,0x00000000,0x00101406,0x00000001,0x05000036,
		0x00102032,0x00000001,0x00101046,0x00000002,0x05000036,0x001020c2,0x00000001,0x00101406,
		0x00000003,0x05000036,0x001020f2,0x00000002,0x00101e46,0x00000004,0x0100003e,0x54415453,
		0x00000074,0x00000006,0x00000000,0x00000000,0x0000000a,0x00000000,0x00000000,0x00000000,
		0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000005,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
	};

	//////////////////////////////////////////////////////////////////////
	// Data for Font_GS

	uint32 WEAKSYM Font_GS_Data[] =
	{
		0x43425844,0xd3dbed80,0x4dbd6273,0x4a97a51c,0x39f47b77,0x00000001,0x00000884,0x00000005,
		0x00000034,0x00000108,0x000001c8,0x0000023c,0x00000808,0x46454452,0x000000cc,0x00000001,
		0x00000048,0x00000001,0x0000001c,0x47530400,0x00008100,0x00000098,0x0000003c,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x6e6f4376,0x6e617473,
		0xab007374,0x0000003c,0x00000001,0x00000060,0x00000040,0x00000000,0x00000000,0x00000078,
		0x00000000,0x00000040,0x00000002,0x00000088,0x00000000,0x6e617254,0x726f6673,0x74614d6d,
		0x00786972,0x00030003,0x00040004,0x00000000,0x00000000,0x7263694d,0x666f736f,0x52282074,
		0x4c482029,0x53204c53,0x65646168,0x6f432072,0x6c69706d,0x36207265,0x392e332e,0x2e303036,
		0x31343731,0xabab0035,0x4e475349,0x000000b8,0x00000005,0x00000008,0x00000080,0x00000000,
		0x00000000,0x00000003,0x00000000,0x00000303,0x0000008f,0x00000000,0x00000000,0x00000003,
		0x00000000,0x00000c0c,0x00000099,0x00000000,0x00000000,0x00000003,0x00000001,0x00000303,
		0x000000a2,0x00000000,0x00000000,0x00000003,0x00000001,0x00000c0c,0x000000ab,0x00000000,
		0x00000000,0x00000003,0x00000002,0x00000f0f,0x616f6c66,0x6f505f74,0x69746973,0x68006e6f,
		0x5f666c61,0x657a6953,0x6c616800,0x56555f66,0x61680061,0x555f666c,0x62006256,0x5f657479,
		0x6f6c6f43,0xabab0072,0x4e47534f,0x0000006c,0x00000003,0x00000008,0x00000050,0x00000000,
		0x00000001,0x00000003,0x00000000,0x0000000f,0x0000005c,0x00000000,0x00000000,0x00000003,
		0x00000001,0x0000000f,0x00000062,0x00000000,0x00000000,0x00000003,0x00000002,0x00000c03,
		0x505f5653,0x7469736f,0x006e6f69,0x4f4c4f43,0x45540052,0x4f4f4358,0xab004452,0x52444853,
		0x000005c4,0x00020040,0x00000171,0x04000059,0x00208e46,0x00000000,0x00000004,0x0400005f,
		0x00201032,0x00000001,0x00000000,0x0400005f,0x002010c2,0x00000001,0x00000000,0x0400005f,
		0x00201032,0x00000001,0x00000001,0x0400005f,0x002010c2,0x00000001,0x00000001,0x0400005f,
		0x002010f2,0x00000001,0x00000002,0x02000068,0x00000002,0x0100085d,0x0100285c,0x04000067,
		0x001020f2,0x00000000,0x00000001,0x03000065,0x001020f2,0x00000001,0x03000065,0x00102032,
		0x00000002,0x0200005e,0x00000004,0x01000009,0x06000036,0x00100032,0x00000000,0x00201046,
		0x00000000,0x00000000,0x05000036,0x00100042,0x00000000,0x00004001,0x3f800000,0x08000010,
		0x00100082,0x00000000,0x00100246,0x00000000,0x00208346,0x00000000,0x00000000,0x05000036,
		0x00102012,0x00000000,0x0010003a,0x00000000,0x08000010,0x00100082,0x00000000,0x00100246,
		0x00000000,0x00208346,0x00000000,0x00000001,0x05000036,0x00102022,0x00000000,0x0010003a,
		0x00000000,0x08000010,0x00100082,0x00000000,0x00100246,0x00000000,0x00208346,0x00000000,
		0x00000002,0x08000010,0x00100012,0x00000000,0x00100246,0x00000000,0x00208346,0x00000000,
		0x00000003,0x05000036,0x00102042,0x00000000,0x0010003a,0x00000000,0x05000036,0x00102082,
		0x00000000,0x0010000a,0x00000000,0x06000036,0x001020f2,0x00000001,0x00201e46,0x00000000,
		0x00000002,0x06000036,0x00102032,0x00000002,0x00201046,0x00000000,0x00000001,0x01000013,
		0x06000036,0x00100092,0x00000000,0x00201ea6,0x00000000,0x00000000,0x08000036,0x00100062,
		0x00000000,0x00004002,0x00000000,0x00000000,0x00000000,0x00000000,0x08000000,0x00100032,
		0x00000001,0x00100046,0x00000000,0x00201046,0x00000000,0x00000000,0x08000000,0x00100032,
		0x00000000,0x00100ae6,0x00000000,0x00201046,0x00000000,0x00000000,0x05000036,0x00100042,
		0x00000001,0x00004001,0x3f800000,0x08000010,0x00100082,0x00000000,0x00100246,0x00000001,
		0x00208346,0x00000000,0x00000000,0x05000036,0x00102012,0x00000000,0x0010003a,0x00000000,
		0x08000010,0x00100082,0x00000000,0x00100246,0x00000001,0x00208346,0x00000000,0x00000001,
		0x05000036,0x00102022,0x00000000,0x0010003a,0x00000000,0x08000010,0x00100082,0x00000000,
		0x00100246,0x00000001,0x00208346,0x00000000,0x00000002,0x08000010,0x00100012,0x00000001,
		0x00100246,0x00000001,0x00208346,0x00000000,0x00000003,0x05000036,0x00102042,0x00000000,
		0x0010003a,0x00000000,0x05000036,0x00102082,0x00000000,0x0010000a,0x00000001,0x06000036,
		0x001020f2,0x00000001,0x00201e46,0x00000000,0x00000002,0x06000036,0x00102032,0x00000002,
		0x00201a66,0x00000000,0x00000001,0x01000013,0x05000036,0x00100042,0x00000000,0x00004001,
		0x3f800000,0x08000010,0x00100082,0x00000000,0x00100246,0x00000000,0x00208346,0x00000000,
		0x00000000,0x05000036,0x00102012,0x00000000,0x0010003a,0x00000000,0x08000010,0x00100082,
		0x00000000,0x00100246,0x00000000,0x00208346,0x00000000,0x00000001,0x05000036,0x00102022,
		0x00000000,0x0010003a,0x00000000,0x08000010,0x00100082,0x00000000,0x00100246,0x00000000,
		0x00208346,0x00000000,0x00000002,0x08000010,0x00100012,0x00000000,0x00100246,0x00000000,
		0x00208346,0x00000000,0x00000003,0x05000036,0x00102042,0x00000000,0x0010003a,0x00000000,
		0x05000036,0x00102082,0x00000000,0x0010000a,0x00000000,0x06000036,0x001020f2,0x00000001,
		0x00201e46,0x00000000,0x00000002,0x06000036,0x00102032,0x00000002,0x002010c6,0x00000000,
		0x00000001,0x01000013,0x09000000,0x00100032,0x00000000,0x00201046,0x00000000,0x00000000,
		0x00201ae6,0x00000000,0x00000000,0x05000036,0x00100042,0x00000000,0x00004001,0x3f800000,
		0x08000010,0x00100082,0x00000000,0x00100246,0x00000000,0x00208346,0x00000000,0x00000000,
		0x05000036,0x00102012,0x00000000,0x0010003a,0x00000000,0x08000010,0x00100082,0x00000000,
		0x00100246,0x00000000,0x00208346,0x00000000,0x00000001,0x05000036,0x00102022,0x00000000,
		0x0010003a,0x00000000,0x08000010,0x00100082,0x00000000,0x00100246,0x00000000,0x00208346,
		0x00000000,0x00000002,0x08000010,0x00100012,0x00000000,0x00100246,0x00000000,0x00208346,
		0x00000000,0x00000003,0x05000036,0x00102042,0x00000000,0x0010003a,0x00000000,0x05000036,
		0x00102082,0x00000000,0x0010000a,0x00000000,0x06000036,0x001020f2,0x00000001,0x00201e46,
		0x00000000,0x00000002,0x06000036,0x00102032,0x00000002,0x00201ae6,0x00000000,0x00000001,
		0x01000013,0x0100003e,0x54415453,0x00000074,0x00000038,0x00000002,0x00000000,0x00000008,
		0x00000013,0x00000000,0x00000000,0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000001,0x00000004,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000007,
		0x00000000,0x00000000,0x00000000,0x00000001,0x00000005,0x00000004,0x00000000,0x00000000,
		0x00000000
	};

	//////////////////////////////////////////////////////////////////////
	// Data for Font_PS

	uint32 WEAKSYM Font_PS_Data[] =
	{
		0x43425844,0xb4f46f3c,0x2c65fdcf,0xd6e5d843,0x869e2286,0x00000001,0x00000298,0x00000005,
		0x00000034,0x000000d8,0x0000014c,0x00000180,0x0000021c,0x46454452,0x0000009c,0x00000000,
		0x00000000,0x00000002,0x0000001c,0xffff0400,0x00008100,0x00000067,0x0000005c,0x00000003,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x00000062,0x00000002,
		0x00000005,0x00000004,0xffffffff,0x00000000,0x00000001,0x0000000c,0x6c706d73,0x61700072,
		0x4d006567,0x6f726369,0x74666f73,0x29522820,0x534c4820,0x6853204c,0x72656461,0x6d6f4320,
		0x656c6970,0x2e362072,0x36392e33,0x312e3030,0x35313437,0xababab00,0x4e475349,0x0000006c,
		0x00000003,0x00000008,0x00000050,0x00000000,0x00000001,0x00000003,0x00000000,0x0000000f,
		0x0000005c,0x00000000,0x00000000,0x00000003,0x00000001,0x00000f0f,0x00000062,0x00000000,
		0x00000000,0x00000003,0x00000002,0x00000303,0x505f5653,0x7469736f,0x006e6f69,0x4f4c4f43,
		0x45540052,0x4f4f4358,0xab004452,0x4e47534f,0x0000002c,0x00000001,0x00000008,0x00000020,
		0x00000000,0x00000000,0x00000003,0x00000000,0x0000000f,0x545f5653,0x45475241,0xabab0054,
		0x52444853,0x00000094,0x00000040,0x00000025,0x0300005a,0x00106000,0x00000000,0x04001858,
		0x00107000,0x00000000,0x00005555,0x03001062,0x001010f2,0x00000001,0x03001062,0x00101032,
		0x00000002,0x03000065,0x001020f2,0x00000000,0x02000068,0x00000001,0x09000045,0x001000f2,
		0x00000000,0x00101046,0x00000002,0x00107e46,0x00000000,0x00106000,0x00000000,0x07000038,
		0x001020f2,0x00000000,0x00100e46,0x00000000,0x00101e46,0x00000001,0x0100003e,0x54415453,
		0x00000074,0x00000003,0x00000001,0x00000000,0x00000003,0x00000001,0x00000000,0x00000000,
		0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
	};

	//////////////////////////////////////////////////////////////////////
	// offsets and defaults

	//////////////////////////////////////////////////////////////////////
	// vConstants offsets

	extern ConstBufferOffset const WEAKSYM Font_GS_vConstants_Offsets[1] =
	{
		{ "TransformMatrix", 0 }
	};

	// no defaults for vConstants

	//////////////////////////////////////////////////////////////////////
	// const buffer names table

	extern char const WEAKSYM *Font_GS_ConstBufferNames[] =
	{
		"vConstants"
	};

	//////////////////////////////////////////////////////////////////////
	// Sampler names

	extern char const WEAKSYM * Font_PS_SamplerNames[] =
	{
		"smplr"
	};

	//////////////////////////////////////////////////////////////////////
	// Texture names

	extern char const WEAKSYM * Font_PS_TextureNames[] =
	{
		"page"
	};

	//////////////////////////////////////////////////////////////////////
	// Input Element descs

	extern D3D11_INPUT_ELEMENT_DESC const WEAKSYM Font_VS_InputElements[5] =
	{
		{ "float_Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "half_Size", 0, DXGI_FORMAT_R16G16_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "half_UVa", 0, DXGI_FORMAT_R16G16_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "half_UVb", 0, DXGI_FORMAT_R16G16_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "byte_Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//////////////////////////////////////////////////////////////////////
	// Shader struct

	struct Font : ShaderState
	{
		//////////////////////////////////////////////////////////////////////
		// InputVertex

		struct InputVertex
		{
			Float2 Position;
			Half2 Size;
			Half2 UVa;
			Half2 UVb;
			Byte4 Color;
		};

		using VertBuffer = VertexBuffer<InputVertex>;

		//////////////////////////////////////////////////////////////////////
		// Vertex Shader

		struct VS : VertexShader, Aligned16
		{
			// Constructor

			VS()
				: VertexShader(Font_VS_Data, 888, 0, null, 0, null, 0, null, null, null, Font_VS_InputElements, _countof(Font_VS_InputElements))
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Geometry Shader

		struct GS : GeometryShader, Aligned16
		{
			// Const Buffers

			struct vConstants_t: Aligned16
			{
				Float4x4 TransformMatrix;
			};

			ConstBuffer<vConstants_t> vConstants;
			enum { vConstants_index = 0 };

			// Constructor

			GS()
				: GeometryShader(Font_GS_Data, 2180, 1, Font_GS_ConstBufferNames, 0, null, 0, null, null, null)
				, vConstants(1, Font_GS_vConstants_Offsets, null, this, 0)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Pixel Shader

		struct PS : PixelShader, Aligned16
		{
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
				: PixelShader(Font_PS_Data, 664, 0, null, 1, Font_PS_SamplerNames, 1, Font_PS_TextureNames, textures, samplers)
				, smplr(null)
				, page(null)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Members

		VS vs;
		GS gs;
		PS ps;

		//////////////////////////////////////////////////////////////////////
		// Constructor

		Font(): ShaderState(Font_BlendDesc, Font_DepthStencilDesc, Font_RasterizerDesc)
		{
			Shaders[Vertex] = &vs;
			Shaders[Hull] = null;
			Shaders[Domain] = null;
			Shaders[Geometry] = &gs;
			Shaders[Pixel] = &ps;
			Shaders[Compute] = null;
		}

		//////////////////////////////////////////////////////////////////////
		// Activate

		void Activate(ID3D11DeviceContext *context)
		{
			ShaderState::SetState(context);
			vs.Activate(context);
			context->HSSetShader(null, null, 0);
			context->DSSetShader(null, null, 0);
			gs.Activate(context);
			ps.Activate(context);
			context->CSSetShader(null, null, 0);
		}

		void Activate_V(ID3D11DeviceContext *context) override
		{
			Activate(context);
		}
	};
}

#pragma pack(pop)
