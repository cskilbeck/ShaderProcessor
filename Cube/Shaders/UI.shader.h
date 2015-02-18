//////////////////////////////////////////////////////////////////////
// UI.h - auto generated file, do not edit

#pragma once
#pragma pack(push, 4)

namespace Shaders
{
	using namespace DX;

	// D3D11_DEPTH_STENCIL_DESC
	uint32 WEAKSYM UI_DepthStencilDesc[] = 
	{
		0x00000000,0x00000000,0x00000002,0x00000000,0x0000ffff,0x00000001,0x00000001,0x00000001,
		0x00000008,0x00000001,0x00000001,0x00000001,0x00000008
	};

	// D3D11_RASTERIZER_DESC
	uint32 WEAKSYM UI_RasterizerDesc[] = 
	{
		0x00000003,0x00000003,0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,
		0x00000000,0x00000000
	};

	// D3D11_BLEND_DESC
	uint32 WEAKSYM UI_BlendDesc[] = 
	{
		0x00000000,0x00000000,0x00000001,0x00000005,0x00000006,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f
	};

	//////////////////////////////////////////////////////////////////////
	// Data for UI_VS

	uint32 WEAKSYM UI_VS_Data[] =
	{
		0x43425844,0x69b9cbcb,0x3efa1a95,0x4d970009,0xf3f0fec3,0x00000001,0x00000360,0x00000005,
		0x00000034,0x00000104,0x00000164,0x000001bc,0x000002e4,0x46454452,0x000000c8,0x00000001,
		0x00000048,0x00000001,0x0000001c,0xfffe0400,0x00008100,0x00000098,0x0000003c,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x6e6f4376,0x6e617473,
		0xab007374,0x0000003c,0x00000001,0x00000060,0x00000040,0x00000000,0x00000000,0x00000078,
		0x00000000,0x00000040,0x00000002,0x00000088,0x00000000,0x6e617254,0x726f6673,0x74614d6d,
		0x00786972,0x00030003,0x00040004,0x00000000,0x00000000,0x7263694d,0x666f736f,0x52282074,
		0x4c482029,0x53204c53,0x65646168,0x6f432072,0x6c69706d,0x31207265,0x2e302e30,0x30303839,
		0xab00302e,0x4e475349,0x00000058,0x00000002,0x00000008,0x00000038,0x00000000,0x00000000,
		0x00000003,0x00000000,0x00000303,0x00000047,0x00000000,0x00000000,0x00000003,0x00000001,
		0x00000303,0x616f6c66,0x6f505f74,0x69746973,0x68006e6f,0x5f666c61,0x43786554,0x64726f6f,
		0xababab00,0x4e47534f,0x00000050,0x00000002,0x00000008,0x00000038,0x00000000,0x00000001,
		0x00000003,0x00000000,0x0000000f,0x00000044,0x00000002,0x00000000,0x00000003,0x00000001,
		0x00000c03,0x505f5653,0x7469736f,0x006e6f69,0x43584554,0x44524f4f,0xababab00,0x52444853,
		0x00000120,0x00010040,0x00000048,0x04000059,0x00208e46,0x00000000,0x00000004,0x0300005f,
		0x00101032,0x00000000,0x0300005f,0x00101032,0x00000001,0x04000067,0x001020f2,0x00000000,
		0x00000001,0x03000065,0x00102032,0x00000001,0x02000068,0x00000001,0x05000036,0x00100032,
		0x00000000,0x00101046,0x00000000,0x08000036,0x001000c2,0x00000000,0x00004002,0x00000000,
		0x00000000,0x3f000000,0x3f800000,0x08000011,0x00102012,0x00000000,0x00100e46,0x00000000,
		0x00208e46,0x00000000,0x00000000,0x08000011,0x00102022,0x00000000,0x00100e46,0x00000000,
		0x00208e46,0x00000000,0x00000001,0x08000011,0x00102042,0x00000000,0x00100e46,0x00000000,
		0x00208e46,0x00000000,0x00000002,0x08000011,0x00102082,0x00000000,0x00100e46,0x00000000,
		0x00208e46,0x00000000,0x00000003,0x05000036,0x00102032,0x00000001,0x00101046,0x00000001,
		0x0100003e,0x54415453,0x00000074,0x00000008,0x00000001,0x00000000,0x00000004,0x00000004,
		0x00000000,0x00000000,0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000003,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
	};

	//////////////////////////////////////////////////////////////////////
	// Data for UI_PS

	uint32 WEAKSYM UI_PS_Data[] =
	{
		0x43425844,0x24d58cba,0x6ff1da5e,0xccacf0a2,0x79daa409,0x00000001,0x000002f4,0x00000005,
		0x00000034,0x00000148,0x000001a0,0x000001d4,0x00000278,0x46454452,0x0000010c,0x00000001,
		0x00000094,0x00000003,0x0000001c,0xffff0400,0x00008100,0x000000dc,0x0000007c,0x00000003,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x00000082,0x00000002,
		0x00000005,0x00000004,0xffffffff,0x00000000,0x00000001,0x0000000c,0x00000087,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x6c706d73,0x61700072,
		0x70006567,0x736e6f43,0x746e6174,0xabab0073,0x00000087,0x00000001,0x000000ac,0x00000010,
		0x00000000,0x00000000,0x000000c4,0x00000000,0x00000010,0x00000002,0x000000cc,0x00000000,
		0x6f6c6f43,0xabab0072,0x00030001,0x00040001,0x00000000,0x00000000,0x7263694d,0x666f736f,
		0x52282074,0x4c482029,0x53204c53,0x65646168,0x6f432072,0x6c69706d,0x31207265,0x2e302e30,
		0x30303839,0xab00302e,0x4e475349,0x00000050,0x00000002,0x00000008,0x00000038,0x00000000,
		0x00000001,0x00000003,0x00000000,0x0000000f,0x00000044,0x00000002,0x00000000,0x00000003,
		0x00000001,0x00000303,0x505f5653,0x7469736f,0x006e6f69,0x43584554,0x44524f4f,0xababab00,
		0x4e47534f,0x0000002c,0x00000001,0x00000008,0x00000020,0x00000000,0x00000000,0x00000003,
		0x00000000,0x0000000f,0x545f5653,0x45475241,0xabab0054,0x52444853,0x0000009c,0x00000040,
		0x00000027,0x04000059,0x00208e46,0x00000000,0x00000001,0x0300005a,0x00106000,0x00000000,
		0x04001858,0x00107000,0x00000000,0x00005555,0x03001062,0x00101032,0x00000001,0x03000065,
		0x001020f2,0x00000000,0x02000068,0x00000001,0x09000045,0x001000f2,0x00000000,0x00101046,
		0x00000001,0x00107e46,0x00000000,0x00106000,0x00000000,0x08000038,0x001020f2,0x00000000,
		0x00100e46,0x00000000,0x00208e46,0x00000000,0x00000000,0x0100003e,0x54415453,0x00000074,
		0x00000003,0x00000001,0x00000000,0x00000002,0x00000001,0x00000000,0x00000000,0x00000001,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
	};

	//////////////////////////////////////////////////////////////////////
	// offsets and defaults

	//////////////////////////////////////////////////////////////////////
	// vConstants offsets

	extern ConstBufferOffset const WEAKSYM UI_VS_vConstants_Offsets[1] =
	{
		{ "TransformMatrix", 0 }
	};

	// no defaults for vConstants

	//////////////////////////////////////////////////////////////////////
	// const buffer names table

	extern char const WEAKSYM *UI_VS_ConstBufferNames[] =
	{
		"vConstants"
	};

	//////////////////////////////////////////////////////////////////////
	// offsets and defaults

	//////////////////////////////////////////////////////////////////////
	// pConstants offsets

	extern ConstBufferOffset const WEAKSYM UI_PS_pConstants_Offsets[1] =
	{
		{ "Color", 0 }
	};

	// no defaults for pConstants

	//////////////////////////////////////////////////////////////////////
	// const buffer names table

	extern char const WEAKSYM *UI_PS_ConstBufferNames[] =
	{
		"pConstants"
	};

	//////////////////////////////////////////////////////////////////////
	// Sampler names

	extern char const WEAKSYM * UI_PS_SamplerNames[] =
	{
		"smplr"
	};

	//////////////////////////////////////////////////////////////////////
	// Texture names

	extern char const WEAKSYM * UI_PS_TextureNames[] =
	{
		"page"
	};

	//////////////////////////////////////////////////////////////////////
	// Input Element descs

	extern D3D11_INPUT_ELEMENT_DESC const WEAKSYM UI_VS_InputElements[2] =
	{
		{ "float_Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "half_TexCoord", 0, DXGI_FORMAT_R16G16_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//////////////////////////////////////////////////////////////////////
	// Shader struct

	struct UI : ShaderState
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
				: VertexShader(UI_VS_Data, 864, 1, UI_VS_ConstBufferNames, 0, null, 0, null, null, null, UI_VS_InputElements, _countof(UI_VS_InputElements))
				, vConstants(1, UI_VS_vConstants_Offsets, null, this, 0)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Pixel Shader

		struct PS : PixelShader, Aligned16
		{
			// Const Buffers

			struct pConstants_t: Aligned16
			{
				Float4 Color;
			};

			ConstBuffer<pConstants_t> pConstants;
			enum { pConstants_index = 0 };

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
				: PixelShader(UI_PS_Data, 756, 1, UI_PS_ConstBufferNames, 1, UI_PS_SamplerNames, 1, UI_PS_TextureNames, textures, samplers)
				, smplr(null)
				, page(null)
				, pConstants(1, UI_PS_pConstants_Offsets, null, this, 0)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Members

		VS vs;
		PS ps;

		DXPtr<ID3D11DepthStencilState> depthStencilState;
		DXPtr<ID3D11RasterizerState> rasterizerState;
		DXPtr<ID3D11BlendState> blendState;

		//////////////////////////////////////////////////////////////////////
		// Constructor

		UI()
		{
			Shaders[Vertex] = &vs;
			Shaders[Hull] = null;
			Shaders[Domain] = null;
			Shaders[Geometry] = null;
			Shaders[Pixel] = &ps;
			Shaders[Compute] = null;

			DX::Device->CreateDepthStencilState((D3D11_DEPTH_STENCIL_DESC const *)UI_DepthStencilDesc, &depthStencilState);
			DX::Device->CreateRasterizerState((D3D11_RASTERIZER_DESC const *)UI_RasterizerDesc, &rasterizerState);
			DX::Device->CreateBlendState((D3D11_BLEND_DESC const *)UI_BlendDesc, &blendState);
		}

		//////////////////////////////////////////////////////////////////////
		// Activate

		void Activate(ID3D11DeviceContext *context)
		{
			context->OMSetDepthStencilState(depthStencilState, 0);
			context->OMSetBlendState(blendState, null, 0xffffffff);
			context->RSSetState(rasterizerState);

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
