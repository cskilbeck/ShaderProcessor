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
	// Binding runs

	extern Shader::BindingState WEAKSYM Blit_Bindings[] = {
		{{}, {}, {{{0,1}}}},
		{},
		{},
		{},
		{{{{0,1}}}, {{{0,1}}}, {}},
		{}
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

		struct VS : VertexShader
		{
			// Const Buffers

			struct vConstants_t
			{
				Float4x4 TransformMatrix;
			};

			ConstBuffer<vConstants_t> vConstants;
			enum { vConstants_index = 0 };

			// Constructor

			VS()
				: VertexShader(1, Blit_VS_ConstBufferNames, 0, null, 0, null, null, null, Blit_Bindings[ShaderType::Vertex])
				, vConstants(1, Blit_VS_vConstants_Offsets, null, this, 0, 0)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Pixel Shader

		struct PS : PixelShader
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
				: PixelShader(0, null, 1, Blit_PS_SamplerNames, 1, Blit_PS_TextureNames, textures, samplers, Blit_Bindings[ShaderType::Pixel])
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
			FileResource f(TEXT("Data\\Blit.sob"));
			vs.Load(f, Blit_VS_InputElements, _countof(Blit_VS_InputElements));
			ps.Load(f);
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
