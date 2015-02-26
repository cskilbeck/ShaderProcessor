//////////////////////////////////////////////////////////////////////
// Blit.h - auto generated file, do not edit

#pragma once
#pragma pack(push, 4)

namespace Shaders
{
	using namespace DX;

	// D3D11_DEPTH_STENCIL_DESC
	uint32 WEAKSYM Blit_DepthStencilDesc[] = 
	{
		0x00000000,0x00000000,0x00000002,0x00000000,0x0000ffff,0x00000001,0x00000001,0x00000001,
		0x00000008,0x00000001,0x00000001,0x00000001,0x00000008
	};

	// D3D11_RASTERIZER_DESC
	uint32 WEAKSYM Blit_RasterizerDesc[] = 
	{
		0x00000003,0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,
		0x00000000,0x00000000
	};

	// D3D11_BLEND_DESC
	uint32 WEAKSYM Blit_BlendDesc[] = 
	{
		0x00000000,0x00000000,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
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
	// Data for Blit_VS

	uint32 WEAKSYM Blit_VS_Data[] =
	{
		0x43425844,0xd1971f45,0x2e1ed908,0xd7c41a77,0x92899ed2,0x00000001,0x00000358,0x00000005,
		0x00000034,0x00000108,0x00000168,0x000001c0,0x000002dc,0x46454452,0x000000cc,0x00000001,
		0x00000048,0x00000001,0x0000001c,0xfffe0400,0x00008100,0x00000098,0x0000003c,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x6e6f4376,0x6e617473,
		0xab007374,0x0000003c,0x00000001,0x00000060,0x00000040,0x00000000,0x00000000,0x00000078,
		0x00000000,0x00000040,0x00000002,0x00000088,0x00000000,0x6e617254,0x726f6673,0x74614d6d,
		0x00786972,0x00030003,0x00040004,0x00000000,0x00000000,0x7263694d,0x666f736f,0x52282074,
		0x4c482029,0x53204c53,0x65646168,0x6f432072,0x6c69706d,0x36207265,0x392e332e,0x2e303036,
		0x31343731,0xabab0035,0x4e475349,0x00000058,0x00000002,0x00000008,0x00000038,0x00000000,
		0x00000000,0x00000003,0x00000000,0x00000303,0x00000047,0x00000000,0x00000000,0x00000003,
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
	// Data for Blit_PS

	uint32 WEAKSYM Blit_PS_Data[] =
	{
		0x43425844,0xfa197e4d,0xbab67d51,0x36f24b8f,0x9098ff2c,0x00000001,0x0000024c,0x00000005,
		0x00000034,0x000000d8,0x00000130,0x00000164,0x000001d0,0x46454452,0x0000009c,0x00000000,
		0x00000000,0x00000002,0x0000001c,0xffff0400,0x00008100,0x00000067,0x0000005c,0x00000003,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x00000062,0x00000002,
		0x00000005,0x00000004,0xffffffff,0x00000000,0x00000001,0x0000000c,0x6c706d73,0x61700072,
		0x4d006567,0x6f726369,0x74666f73,0x29522820,0x534c4820,0x6853204c,0x72656461,0x6d6f4320,
		0x656c6970,0x2e362072,0x36392e33,0x312e3030,0x35313437,0xababab00,0x4e475349,0x00000050,
		0x00000002,0x00000008,0x00000038,0x00000000,0x00000001,0x00000003,0x00000000,0x0000000f,
		0x00000044,0x00000002,0x00000000,0x00000003,0x00000001,0x00000303,0x505f5653,0x7469736f,
		0x006e6f69,0x43584554,0x44524f4f,0xababab00,0x4e47534f,0x0000002c,0x00000001,0x00000008,
		0x00000020,0x00000000,0x00000000,0x00000003,0x00000000,0x0000000f,0x545f5653,0x45475241,
		0xabab0054,0x52444853,0x00000064,0x00000040,0x00000019,0x0300005a,0x00106000,0x00000000,
		0x04001858,0x00107000,0x00000000,0x00005555,0x03001062,0x00101032,0x00000001,0x03000065,
		0x001020f2,0x00000000,0x09000045,0x001020f2,0x00000000,0x00101046,0x00000001,0x00107e46,
		0x00000000,0x00106000,0x00000000,0x0100003e,0x54415453,0x00000074,0x00000002,0x00000000,
		0x00000000,0x00000002,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000
	};

	//////////////////////////////////////////////////////////////////////
	// offsets and defaults

	//////////////////////////////////////////////////////////////////////
	// vConstants offsets

	extern ConstBufferOffset const WEAKSYM Blit_VS_vConstants_Offsets[1] =
	{
		{ "TransformMatrix", 0 }
	};

	// no defaults for vConstants

	//////////////////////////////////////////////////////////////////////
	// const buffer names table

	extern char const WEAKSYM *Blit_VS_ConstBufferNames[] =
	{
		"vConstants"
	};

	//////////////////////////////////////////////////////////////////////
	// Sampler names

	extern char const WEAKSYM * Blit_PS_SamplerNames[] =
	{
		"smplr"
	};

	//////////////////////////////////////////////////////////////////////
	// Texture names

	extern char const WEAKSYM * Blit_PS_TextureNames[] =
	{
		"page"
	};

	//////////////////////////////////////////////////////////////////////
	// Input Element descs

	extern D3D11_INPUT_ELEMENT_DESC const WEAKSYM Blit_VS_InputElements[2] =
	{
		{ "float_Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "half_TexCoord", 0, DXGI_FORMAT_R16G16_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//////////////////////////////////////////////////////////////////////
	// Shader struct

	struct Blit : ShaderState
	{
		//////////////////////////////////////////////////////////////////////
		// InputVertex

		struct InputVertex
		{
			Float2 Position;
			Half2 TexCoord;
		};

		using VertBuffer = VertexBuffer<InputVertex>;

		//////////////////////////////////////////////////////////////////////
		// Vertex Shader

		struct VS : VertexShader, Aligned16
		{
			// Const Buffers

			struct vConstants_t: Aligned16
			{
				Float4x4 TransformMatrix;
			};

			ConstBuffer<vConstants_t> vConstants;
			enum { vConstants_index = 0 };

			// Constructor

			VS()
				: VertexShader(Blit_VS_Data, 856, 1, Blit_VS_ConstBufferNames, 0, null, 0, null, null, null, Blit_VS_InputElements, _countof(Blit_VS_InputElements))
				, vConstants(1, Blit_VS_vConstants_Offsets, null, this, 0)
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
				: PixelShader(Blit_PS_Data, 588, 0, null, 1, Blit_PS_SamplerNames, 1, Blit_PS_TextureNames, textures, samplers)
				, smplr(null)
				, page(null)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Members

		VS vs;
		PS ps;

		//////////////////////////////////////////////////////////////////////
		// Constructor

		Blit(): ShaderState(Blit_BlendDesc, Blit_DepthStencilDesc, Blit_RasterizerDesc)
		{
			Shaders[Vertex] = &vs;
			Shaders[Hull] = null;
			Shaders[Domain] = null;
			Shaders[Geometry] = null;
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
			context->GSSetShader(null, null, 0);
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
