//////////////////////////////////////////////////////////////////////
// Simple.h - auto generated file, do not edit

#pragma once
#pragma pack(push, 4)

namespace Shaders
{
	using namespace DX;

	// D3D11_DEPTH_STENCIL_DESC
	uint32 WEAKSYM Simple_DepthStencilDesc[] = 
	{
		0x00000001,0x00000000,0x00000002,0x00000000,0x0000ffff,0x00000001,0x00000001,0x00000001,
		0x00000008,0x00000001,0x00000001,0x00000001,0x00000008
	};

	// D3D11_RASTERIZER_DESC
	uint32 WEAKSYM Simple_RasterizerDesc[] = 
	{
		0x00000003,0x00000003,0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,
		0x00000000,0x00000000
	};

	// D3D11_BLEND_DESC
	uint32 WEAKSYM Simple_BlendDesc[] = 
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
	// Data for Simple_VS

	uint32 WEAKSYM Simple_VS_Data[] =
	{
		0x43425844,0x3ad0cf17,0x37c060ad,0x5b520774,0xba0a5be3,0x00000001,0x00000354,0x00000005,
		0x00000034,0x0000010c,0x00000168,0x000001bc,0x000002d8,0x46454452,0x000000d0,0x00000001,
		0x0000004c,0x00000001,0x0000001c,0xfffe0400,0x00008100,0x0000009c,0x0000003c,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x74726556,0x736e6f43,
		0x746e6174,0xabab0073,0x0000003c,0x00000001,0x00000064,0x00000040,0x00000000,0x00000000,
		0x0000007c,0x00000000,0x00000040,0x00000002,0x0000008c,0x00000000,0x6e617254,0x726f6673,
		0x74614d6d,0x00786972,0x00030003,0x00040004,0x00000000,0x00000000,0x7263694d,0x666f736f,
		0x52282074,0x4c482029,0x53204c53,0x65646168,0x6f432072,0x6c69706d,0x36207265,0x392e332e,
		0x2e303036,0x31343731,0xabab0035,0x4e475349,0x00000054,0x00000002,0x00000008,0x00000038,
		0x00000000,0x00000000,0x00000003,0x00000000,0x00000707,0x00000047,0x00000000,0x00000000,
		0x00000003,0x00000001,0x00000f0f,0x616f6c66,0x6f505f74,0x69746973,0x62006e6f,0x5f657479,
		0x6f6c6f43,0xabab0072,0x4e47534f,0x0000004c,0x00000002,0x00000008,0x00000038,0x00000000,
		0x00000001,0x00000003,0x00000000,0x0000000f,0x00000044,0x00000000,0x00000000,0x00000003,
		0x00000001,0x0000000f,0x505f5653,0x7469736f,0x006e6f69,0x4f4c4f43,0xabab0052,0x52444853,
		0x00000114,0x00010040,0x00000045,0x04000059,0x00208e46,0x00000000,0x00000004,0x0300005f,
		0x00101072,0x00000000,0x0300005f,0x001010f2,0x00000001,0x04000067,0x001020f2,0x00000000,
		0x00000001,0x03000065,0x001020f2,0x00000001,0x02000068,0x00000001,0x05000036,0x00100072,
		0x00000000,0x00101246,0x00000000,0x05000036,0x00100082,0x00000000,0x00004001,0x3f800000,
		0x08000011,0x00102012,0x00000000,0x00100e46,0x00000000,0x00208e46,0x00000000,0x00000000,
		0x08000011,0x00102022,0x00000000,0x00100e46,0x00000000,0x00208e46,0x00000000,0x00000001,
		0x08000011,0x00102042,0x00000000,0x00100e46,0x00000000,0x00208e46,0x00000000,0x00000002,
		0x08000011,0x00102082,0x00000000,0x00100e46,0x00000000,0x00208e46,0x00000000,0x00000003,
		0x05000036,0x001020f2,0x00000001,0x00101e46,0x00000001,0x0100003e,0x54415453,0x00000074,
		0x00000008,0x00000001,0x00000000,0x00000004,0x00000004,0x00000000,0x00000000,0x00000001,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000003,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
	};

	//////////////////////////////////////////////////////////////////////
	// Data for Simple_PS

	uint32 WEAKSYM Simple_PS_Data[] =
	{
		0x43425844,0xd54b313e,0x331f8dde,0xed7dcd56,0xa7cb5039,0x00000001,0x000001d0,0x00000005,
		0x00000034,0x0000008c,0x000000e0,0x00000114,0x00000154,0x46454452,0x00000050,0x00000000,
		0x00000000,0x00000000,0x0000001c,0xffff0400,0x00008100,0x0000001c,0x7263694d,0x666f736f,
		0x52282074,0x4c482029,0x53204c53,0x65646168,0x6f432072,0x6c69706d,0x36207265,0x392e332e,
		0x2e303036,0x31343731,0xabab0035,0x4e475349,0x0000004c,0x00000002,0x00000008,0x00000038,
		0x00000000,0x00000001,0x00000003,0x00000000,0x0000000f,0x00000044,0x00000000,0x00000000,
		0x00000003,0x00000001,0x00000f0f,0x505f5653,0x7469736f,0x006e6f69,0x4f4c4f43,0xabab0052,
		0x4e47534f,0x0000002c,0x00000001,0x00000008,0x00000020,0x00000000,0x00000000,0x00000003,
		0x00000000,0x0000000f,0x545f5653,0x45475241,0xabab0054,0x52444853,0x00000038,0x00000040,
		0x0000000e,0x03001062,0x001010f2,0x00000001,0x03000065,0x001020f2,0x00000000,0x05000036,
		0x001020f2,0x00000000,0x00101e46,0x00000001,0x0100003e,0x54415453,0x00000074,0x00000002,
		0x00000000,0x00000000,0x00000002,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000
	};

	//////////////////////////////////////////////////////////////////////
	// offsets and defaults

	//////////////////////////////////////////////////////////////////////
	// VertConstants offsets

	extern ConstBufferOffset const WEAKSYM Simple_VS_VertConstants_Offsets[1] =
	{
		{ "TransformMatrix", 0 }
	};

	// no defaults for VertConstants

	//////////////////////////////////////////////////////////////////////
	// const buffer names table

	extern char const WEAKSYM *Simple_VS_ConstBufferNames[] =
	{
		"VertConstants"
	};

	//////////////////////////////////////////////////////////////////////
	// Input Element descs

	extern D3D11_INPUT_ELEMENT_DESC const WEAKSYM Simple_VS_InputElements[2] =
	{
		{ "float_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "byte_Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//////////////////////////////////////////////////////////////////////
	// Shader struct

	struct Simple : ShaderState
	{
		//////////////////////////////////////////////////////////////////////
		// InputVertex

		struct InputVertex
		{
			Float3 Position;
			Byte4 Color;
		};

		using VertBuffer = VertexBuffer<InputVertex>;

		//////////////////////////////////////////////////////////////////////
		// Vertex Shader

		struct VS : VertexShader, Aligned16
		{
			// Const Buffers

			struct VertConstants_t: Aligned16
			{
				Float4x4 TransformMatrix;
			};

			ConstBuffer<VertConstants_t> VertConstants;
			enum { VertConstants_index = 0 };

			// Constructor

			VS()
				: VertexShader(Simple_VS_Data, 852, 1, Simple_VS_ConstBufferNames, 0, null, 0, null, null, null, Simple_VS_InputElements, _countof(Simple_VS_InputElements))
				, VertConstants(1, Simple_VS_VertConstants_Offsets, null, this, 0)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Pixel Shader

		struct PS : PixelShader, Aligned16
		{
			// Constructor

			PS()
				: PixelShader(Simple_PS_Data, 464, 0, null, 0, null, 0, null, null, null)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Members

		VS vs;
		PS ps;

		//////////////////////////////////////////////////////////////////////
		// Constructor

		Simple(): ShaderState(Simple_BlendDesc, Simple_DepthStencilDesc, Simple_RasterizerDesc)
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
