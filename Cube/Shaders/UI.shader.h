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

	// Data for UI_VS is in Data\UI_VS.cso

	// Data for UI_PS is in Data\UI_PS.cso

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
				: VertexShader(TEXT("Data\\UI_VS.cso"), 1, UI_VS_ConstBufferNames, 0, null, 0, null, null, null, UI_VS_InputElements, _countof(UI_VS_InputElements))
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
				: PixelShader(TEXT("Data\\UI_PS.cso"), 1, UI_PS_ConstBufferNames, 1, UI_PS_SamplerNames, 1, UI_PS_TextureNames, textures, samplers)
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

		//////////////////////////////////////////////////////////////////////
		// Constructor

		UI(): ShaderState(UI_BlendDesc, UI_DepthStencilDesc, UI_RasterizerDesc)
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
