//////////////////////////////////////////////////////////////////////
// shader.h - auto generated file, do not edit

#pragma once
#pragma pack(push, 4)

//////////////////////////////////////////////////////////////////////
// vs_shader data

uint32 WEAKSYM vs_shader_Data[] =
{
	0x43425844,0x56a14ca6,0x7b2a7f76,0xa080bf88,0x24b48b33,0x00000001,0x00000418,0x00000005,
	0x00000034,0x00000150,0x000001d4,0x00000248,0x0000039c,0x46454452,0x00000114,0x00000001,
	0x0000004c,0x00000001,0x0000001c,0xfffe0400,0x00000100,0x000000e0,0x0000003c,0x00000000,
	0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x74726556,0x736e6f43,
	0x746e6174,0xabab0073,0x0000003c,0x00000001,0x00000064,0x00000040,0x00000000,0x00000000,
	0x0000007c,0x00000000,0x00000040,0x00000002,0x00000090,0x000000a0,0x6a6f7250,0x69746365,
	0x614d6e6f,0x78697274,0xababab00,0x00030003,0x00040004,0x00000000,0x00000000,0x3f800000,
	0x00000000,0x00000000,0x00000000,0x00000000,0x3f800000,0x00000000,0x00000000,0x00000000,
	0x00000000,0x3f800000,0x00000000,0x00000000,0x00000000,0x00000000,0x3f800000,0x7263694d,
	0x666f736f,0x52282074,0x4c482029,0x53204c53,0x65646168,0x6f432072,0x6c69706d,0x36207265,
	0x392e332e,0x2e303036,0x38333631,0xabab0034,0x4e475349,0x0000007c,0x00000003,0x00000008,
	0x00000050,0x00000000,0x00000000,0x00000003,0x00000000,0x00000303,0x0000005f,0x00000000,
	0x00000000,0x00000003,0x00000001,0x00000303,0x0000006e,0x00000000,0x00000000,0x00000003,
	0x00000002,0x00000f0f,0x616f6c66,0x6f505f74,0x69746973,0x66006e6f,0x74616f6c,0x7865545f,
	0x726f6f43,0x79620064,0x435f6574,0x726f6c6f,0xababab00,0x4e47534f,0x0000006c,0x00000003,
	0x00000008,0x00000050,0x00000000,0x00000001,0x00000003,0x00000000,0x0000000f,0x0000005c,
	0x00000000,0x00000000,0x00000003,0x00000001,0x00000c03,0x00000065,0x00000000,0x00000000,
	0x00000003,0x00000002,0x0000000f,0x505f5653,0x7469736f,0x006e6f69,0x43584554,0x44524f4f,
	0x4c4f4300,0xab00524f,0x52444853,0x0000014c,0x00010040,0x00000053,0x04000059,0x00208e46,
	0x00000000,0x00000004,0x0300005f,0x00101032,0x00000000,0x0300005f,0x00101032,0x00000001,
	0x0300005f,0x001010f2,0x00000002,0x04000067,0x001020f2,0x00000000,0x00000001,0x03000065,
	0x00102032,0x00000001,0x03000065,0x001020f2,0x00000002,0x02000068,0x00000001,0x05000036,
	0x00100032,0x00000000,0x00101046,0x00000000,0x08000036,0x001000c2,0x00000000,0x00004002,
	0x00000000,0x00000000,0x3f000000,0x3f800000,0x08000011,0x00102012,0x00000000,0x00100e46,
	0x00000000,0x00208e46,0x00000000,0x00000000,0x08000011,0x00102022,0x00000000,0x00100e46,
	0x00000000,0x00208e46,0x00000000,0x00000001,0x08000011,0x00102042,0x00000000,0x00100e46,
	0x00000000,0x00208e46,0x00000000,0x00000002,0x08000011,0x00102082,0x00000000,0x00100e46,
	0x00000000,0x00208e46,0x00000000,0x00000003,0x05000036,0x00102032,0x00000001,0x00101046,
	0x00000001,0x05000036,0x001020f2,0x00000002,0x00101e46,0x00000002,0x0100003e,0x54415453,
	0x00000074,0x00000009,0x00000001,0x00000000,0x00000006,0x00000004,0x00000000,0x00000000,
	0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x00000000,0x00000000,0x00000004,0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
};

//////////////////////////////////////////////////////////////////////
// offsets and defaults

// VertConstants offsets
extern ConstBufferOffset const WEAKSYM vs_shader_VertConstants_Offsets[1] = 
{
	{ "ProjectionMatrix", 0 }
};

// VertConstants defaults
extern uint32 ALIGNED(16) WEAKSYM vs_shader_VertConstants_Defaults[16] =
{
	// ProjectionMatrix
	0x3f800000,0x00000000,0x00000000,0x00000000,
	0x00000000,0x3f800000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x3f800000,0x00000000,
	0x00000000,0x00000000,0x00000000,0x3f800000
};

//////////////////////////////////////////////////////////////////////
// const buffer names table

extern char const WEAKSYM *vs_shader_ConstBufferNames[] =
{
	"VertConstants"
};

//////////////////////////////////////////////////////////////////////
// Input Element descs

extern D3D11_INPUT_ELEMENT_DESC const WEAKSYM vs_shader_InputElements[3] =
{
	{ "float_Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "float_TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "byte_Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

//////////////////////////////////////////////////////////////////////
// Vertex Shader: vs_shader

struct vs_shader : VertexShader<vs_shader_InputElements, _countof(vs_shader_InputElements)>, Aligned16
{
	struct ALIGNED(16) VertConstants
	{
		Float4x4 ProjectionMatrix;
	};

	ConstBuffer<VertConstants> VertConstants;

	struct InputVertex
	{
		Float2 Position;
		Float2 TexCoord;
		Byte4 Color;
	};

	using VertexBuffer = VertexBuffer<InputVertex>;

	// Constructor
	vs_shader()
		: VertexShader(vs_shader_Data, 1048, 1, vs_shader_ConstBufferNames, 0, null, 0, null, null, null)
		, VertConstants(1, vs_shader_VertConstants_Offsets, vs_shader_VertConstants_Defaults, this)
	{
	}
};

//////////////////////////////////////////////////////////////////////
// ps_shader data

uint32 WEAKSYM ps_shader_Data[] =
{
	0x43425844,0xf425a7a9,0x67e5dbd5,0x3b4fb2a5,0x1f07a546,0x00000001,0x00000348,0x00000005,
	0x00000034,0x00000158,0x000001cc,0x00000200,0x000002cc,0x46454452,0x0000011c,0x00000001,
	0x000000a0,0x00000003,0x0000001c,0xffff0400,0x00000100,0x000000e8,0x0000007c,0x00000003,
	0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x00000088,0x00000002,
	0x00000005,0x00000004,0xffffffff,0x00000000,0x00000001,0x0000000c,0x00000093,0x00000000,
	0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x31786574,0x706d6153,
	0x0072656c,0x54636970,0x75747865,0x43006572,0x756f6c6f,0x75745372,0xab006666,0x00000093,
	0x00000001,0x000000b8,0x00000010,0x00000000,0x00000000,0x000000d0,0x00000000,0x00000010,
	0x00000002,0x000000d8,0x00000000,0x746e6974,0xababab00,0x00030001,0x00040001,0x00000000,
	0x00000000,0x7263694d,0x666f736f,0x52282074,0x4c482029,0x53204c53,0x65646168,0x6f432072,
	0x6c69706d,0x36207265,0x392e332e,0x2e303036,0x38333631,0xabab0034,0x4e475349,0x0000006c,
	0x00000003,0x00000008,0x00000050,0x00000000,0x00000001,0x00000003,0x00000000,0x0000000f,
	0x0000005c,0x00000000,0x00000000,0x00000003,0x00000001,0x00000303,0x00000065,0x00000000,
	0x00000000,0x00000003,0x00000002,0x00000f0f,0x505f5653,0x7469736f,0x006e6f69,0x43584554,
	0x44524f4f,0x4c4f4300,0xab00524f,0x4e47534f,0x0000002c,0x00000001,0x00000008,0x00000020,
	0x00000000,0x00000000,0x00000003,0x00000000,0x0000000f,0x545f5653,0x45475241,0xabab0054,
	0x52444853,0x000000c4,0x00000040,0x00000031,0x04000059,0x00208e46,0x00000000,0x00000001,
	0x0300005a,0x00106000,0x00000000,0x04001858,0x00107000,0x00000000,0x00005555,0x03001062,
	0x00101032,0x00000001,0x03001062,0x001010f2,0x00000002,0x03000065,0x001020f2,0x00000000,
	0x02000068,0x00000001,0x09000045,0x001000f2,0x00000000,0x00101046,0x00000001,0x00107e46,
	0x00000000,0x00106000,0x00000000,0x07000038,0x001000f2,0x00000000,0x00100e46,0x00000000,
	0x00101e46,0x00000002,0x08000038,0x001020f2,0x00000000,0x00100e46,0x00000000,0x00208e46,
	0x00000000,0x00000000,0x0100003e,0x54415453,0x00000074,0x00000004,0x00000001,0x00000000,
	0x00000003,0x00000002,0x00000000,0x00000000,0x00000001,0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000
};

//////////////////////////////////////////////////////////////////////
// offsets and defaults

// ColourStuff offsets
extern ConstBufferOffset const WEAKSYM ps_shader_ColourStuff_Offsets[1] = 
{
	{ "tint", 0 }
};

// no defaults for ColourStuff

//////////////////////////////////////////////////////////////////////
// const buffer names table

extern char const WEAKSYM *ps_shader_ConstBufferNames[] =
{
	"ColourStuff"
};

//////////////////////////////////////////////////////////////////////
// Sampler names

extern char const WEAKSYM * ps_shader_SamplerNames[] =
{
	"tex1Sampler"
};

//////////////////////////////////////////////////////////////////////
// Texture names

extern char const WEAKSYM * ps_shader_TextureNames[] =
{
	"picTexture"
};

//////////////////////////////////////////////////////////////////////
// Pixel Shader: ps_shader

struct ps_shader : PixelShader, Aligned16
{
	struct ALIGNED(16) ColourStuff
	{
		Float4 tint;
	};

	ConstBuffer<ColourStuff> ColourStuff;

	union
	{
		struct
		{
			Sampler *tex1Sampler;
		};
		Sampler *samplers[1];
	};

	union
	{
		struct
		{
			Texture *picTexture;
		};
		Texture *textures[1];
	};

	// Constructor
	ps_shader()
		: PixelShader(ps_shader_Data, 840, 1, ps_shader_ConstBufferNames, 1, ps_shader_SamplerNames, 1, ps_shader_TextureNames, textures, samplers)
		, tex1Sampler(null)
		, picTexture(null)
		, ColourStuff(1, ps_shader_ColourStuff_Offsets, null, this)
	{
	}
};

#pragma pack(pop)

