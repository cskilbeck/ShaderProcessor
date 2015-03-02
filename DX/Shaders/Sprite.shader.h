//////////////////////////////////////////////////////////////////////
// Sprite.h - auto generated file, do not edit

#pragma once
#pragma pack(push, 4)

namespace Shaders
{
	using namespace DX;

	// D3D11_DEPTH_STENCIL_DESC
	uint32 WEAKSYM Sprite_DepthStencilDesc[] = 
	{
		0x00000000,0x00000000,0x00000002,0x00000000,0x0000ffff,0x00000001,0x00000001,0x00000001,
		0x00000008,0x00000001,0x00000001,0x00000001,0x00000008
	};

	// D3D11_RASTERIZER_DESC
	uint32 WEAKSYM Sprite_RasterizerDesc[] = 
	{
		0x00000003,0x00000003,0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,
		0x00000000,0x00000000
	};

	// D3D11_BLEND_DESC
	uint32 WEAKSYM Sprite_BlendDesc[] = 
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
	// Data for Sprite_VS

	uint32 WEAKSYM Sprite_VS_Data[] =
	{
		0x43425844,0xe8609a21,0x27a18a0d,0x330be79b,0xaf8264df,0x00000001,0x00000550,0x00000005,
		0x00000034,0x0000008c,0x000001e0,0x00000334,0x000004d4,0x46454452,0x00000050,0x00000000,
		0x00000000,0x00000000,0x0000001c,0xfffe0400,0x00008100,0x0000001c,0x7263694d,0x666f736f,
		0x52282074,0x4c482029,0x53204c53,0x65646168,0x6f432072,0x6c69706d,0x36207265,0x392e332e,
		0x2e303036,0x38333631,0xabab0034,0x4e475349,0x0000014c,0x00000009,0x00000008,0x000000e0,
		0x00000000,0x00000000,0x00000003,0x00000000,0x00000303,0x000000ef,0x00000000,0x00000000,
		0x00000003,0x00000001,0x00000303,0x000000fb,0x00000000,0x00000000,0x00000003,0x00000002,
		0x00000303,0x00000106,0x00000000,0x00000000,0x00000003,0x00000003,0x00000303,0x00000112,
		0x00000000,0x00000000,0x00000003,0x00000004,0x00000303,0x0000011c,0x00000000,0x00000000,
		0x00000003,0x00000005,0x00000303,0x00000126,0x00000000,0x00000000,0x00000003,0x00000006,
		0x00000101,0x00000135,0x00000000,0x00000000,0x00000003,0x00000007,0x00000f0f,0x00000140,
		0x00000000,0x00000000,0x00000003,0x00000008,0x00000f0f,0x616f6c66,0x6f505f74,0x69746973,
		0x66006e6f,0x74616f6c,0x7669505f,0x6600746f,0x74616f6c,0x7a69535f,0x6c660065,0x5f74616f,
		0x6c616353,0x6c660065,0x5f74616f,0x00615655,0x616f6c66,0x56555f74,0x6c660062,0x5f74616f,
		0x61746f52,0x6e6f6974,0x74796200,0x6f435f65,0x00726f6c,0x6d726f6e,0x6c465f38,0xab007069,
		0x4e47534f,0x0000014c,0x00000009,0x00000008,0x000000e0,0x00000000,0x00000000,0x00000003,
		0x00000000,0x00000c03,0x000000ef,0x00000000,0x00000000,0x00000003,0x00000000,0x0000030c,
		0x000000fb,0x00000000,0x00000000,0x00000003,0x00000001,0x00000c03,0x00000106,0x00000000,
		0x00000000,0x00000003,0x00000001,0x0000030c,0x00000112,0x00000000,0x00000000,0x00000003,
		0x00000002,0x00000c03,0x0000011c,0x00000000,0x00000000,0x00000003,0x00000002,0x0000030c,
		0x00000126,0x00000000,0x00000000,0x00000003,0x00000003,0x00000e01,0x00000135,0x00000000,
		0x00000000,0x00000003,0x00000004,0x0000000f,0x00000140,0x00000000,0x00000000,0x00000003,
		0x00000005,0x0000000f,0x616f6c66,0x6f505f74,0x69746973,0x66006e6f,0x74616f6c,0x7669505f,
		0x6600746f,0x74616f6c,0x7a69535f,0x6c660065,0x5f74616f,0x6c616353,0x6c660065,0x5f74616f,
		0x00615655,0x616f6c66,0x56555f74,0x6c660062,0x5f74616f,0x61746f52,0x6e6f6974,0x74796200,
		0x6f435f65,0x00726f6c,0x6d726f6e,0x6c465f38,0xab007069,0x52444853,0x00000198,0x00010040,
		0x00000066,0x0300005f,0x00101032,0x00000000,0x0300005f,0x00101032,0x00000001,0x0300005f,
		0x00101032,0x00000002,0x0300005f,0x00101032,0x00000003,0x0300005f,0x00101032,0x00000004,
		0x0300005f,0x00101032,0x00000005,0x0300005f,0x00101012,0x00000006,0x0300005f,0x001010f2,
		0x00000007,0x0300005f,0x001010f2,0x00000008,0x03000065,0x00102032,0x00000000,0x03000065,
		0x001020c2,0x00000000,0x03000065,0x00102032,0x00000001,0x03000065,0x001020c2,0x00000001,
		0x03000065,0x00102032,0x00000002,0x03000065,0x001020c2,0x00000002,0x03000065,0x00102012,
		0x00000003,0x03000065,0x001020f2,0x00000004,0x03000065,0x001020f2,0x00000005,0x05000036,
		0x00102032,0x00000000,0x00101046,0x00000000,0x05000036,0x001020c2,0x00000000,0x00101406,
		0x00000001,0x05000036,0x00102032,0x00000001,0x00101046,0x00000002,0x05000036,0x001020c2,
		0x00000001,0x00101406,0x00000003,0x05000036,0x00102032,0x00000002,0x00101046,0x00000004,
		0x05000036,0x001020c2,0x00000002,0x00101406,0x00000005,0x05000036,0x00102012,0x00000003,
		0x0010100a,0x00000006,0x05000036,0x001020f2,0x00000004,0x00101e46,0x00000007,0x05000036,
		0x001020f2,0x00000005,0x00101e46,0x00000008,0x0100003e,0x54415453,0x00000074,0x0000000a,
		0x00000000,0x00000000,0x00000012,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000009,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000
	};

	//////////////////////////////////////////////////////////////////////
	// Data for Sprite_GS

	uint32 WEAKSYM Sprite_GS_Data[] =
	{
		0x43425844,0x432293f3,0xddcf9d09,0xc7505c9e,0xf60ad43c,0x00000001,0x00000bb4,0x00000005,
		0x00000034,0x00000108,0x0000025c,0x000002d0,0x00000b38,0x46454452,0x000000cc,0x00000001,
		0x00000048,0x00000001,0x0000001c,0x47530400,0x00008100,0x00000098,0x0000003c,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x6e6f4376,0x6e617473,
		0xab007374,0x0000003c,0x00000001,0x00000060,0x00000040,0x00000000,0x00000000,0x00000078,
		0x00000000,0x00000040,0x00000002,0x00000088,0x00000000,0x6e617254,0x726f6673,0x74614d6d,
		0x00786972,0x00030003,0x00040004,0x00000000,0x00000000,0x7263694d,0x666f736f,0x52282074,
		0x4c482029,0x53204c53,0x65646168,0x6f432072,0x6c69706d,0x36207265,0x392e332e,0x2e303036,
		0x38333631,0xabab0034,0x4e475349,0x0000014c,0x00000009,0x00000008,0x000000e0,0x00000000,
		0x00000000,0x00000003,0x00000000,0x00000303,0x000000ef,0x00000000,0x00000000,0x00000003,
		0x00000000,0x00000c0c,0x000000fb,0x00000000,0x00000000,0x00000003,0x00000001,0x00000303,
		0x00000106,0x00000000,0x00000000,0x00000003,0x00000001,0x00000c0c,0x00000112,0x00000000,
		0x00000000,0x00000003,0x00000002,0x00000303,0x0000011c,0x00000000,0x00000000,0x00000003,
		0x00000002,0x00000c0c,0x00000126,0x00000000,0x00000000,0x00000003,0x00000003,0x00000101,
		0x00000135,0x00000000,0x00000000,0x00000003,0x00000004,0x00000f0f,0x00000140,0x00000000,
		0x00000000,0x00000003,0x00000005,0x00000f0f,0x616f6c66,0x6f505f74,0x69746973,0x66006e6f,
		0x74616f6c,0x7669505f,0x6600746f,0x74616f6c,0x7a69535f,0x6c660065,0x5f74616f,0x6c616353,
		0x6c660065,0x5f74616f,0x00615655,0x616f6c66,0x56555f74,0x6c660062,0x5f74616f,0x61746f52,
		0x6e6f6974,0x74796200,0x6f435f65,0x00726f6c,0x6d726f6e,0x6c465f38,0xab007069,0x4e47534f,
		0x0000006c,0x00000003,0x00000008,0x00000050,0x00000000,0x00000001,0x00000003,0x00000000,
		0x0000000f,0x0000005c,0x00000000,0x00000000,0x00000003,0x00000001,0x0000000f,0x00000062,
		0x00000000,0x00000000,0x00000003,0x00000002,0x00000c03,0x505f5653,0x7469736f,0x006e6f69,
		0x4f4c4f43,0x45540052,0x4f4f4358,0xab004452,0x52444853,0x00000860,0x00020040,0x00000218,
		0x04000059,0x00208e46,0x00000000,0x00000004,0x0400005f,0x00201032,0x00000001,0x00000000,
		0x0400005f,0x002010c2,0x00000001,0x00000000,0x0400005f,0x00201032,0x00000001,0x00000001,
		0x0400005f,0x002010c2,0x00000001,0x00000001,0x0400005f,0x00201032,0x00000001,0x00000002,
		0x0400005f,0x002010c2,0x00000001,0x00000002,0x0400005f,0x00201012,0x00000001,0x00000003,
		0x0400005f,0x002010f2,0x00000001,0x00000004,0x0400005f,0x002010f2,0x00000001,0x00000005,
		0x02000068,0x00000006,0x0100085d,0x0100285c,0x04000067,0x001020f2,0x00000000,0x00000001,
		0x03000065,0x001020f2,0x00000001,0x03000065,0x00102032,0x00000002,0x0200005e,0x00000004,
		0x01000009,0x09000038,0x00100032,0x00000000,0x00201ae6,0x00000000,0x00000000,0x00201046,
		0x00000000,0x00000001,0x06000036,0x001000c2,0x00000000,0x80100406,0x00000041,0x00000000,
		0x0d000032,0x00100032,0x00000000,0x80201ae6,0x00000041,0x00000000,0x00000000,0x00201046,
		0x00000000,0x00000001,0x00201046,0x00000000,0x00000001,0x08000038,0x001000f2,0x00000001,
		0x001003b6,0x00000000,0x00201bb6,0x00000000,0x00000001,0x08000038,0x001000f2,0x00000000,
		0x00100196,0x00000000,0x00201bb6,0x00000000,0x00000001,0x0800004d,0x00100012,0x00000002,
		0x00100012,0x00000003,0x0020100a,0x00000000,0x00000003,0x07000038,0x001000f2,0x00000004,
		0x00100e46,0x00000001,0x00100006,0x00000002,0x0a000032,0x00100052,0x00000005,0x00100756,
		0x00000001,0x00100006,0x00000003,0x80100206,0x00000041,0x00000004,0x09000032,0x001000a2,
		0x00000005,0x00100806,0x00000001,0x00100006,0x00000003,0x00100d56,0x00000004,0x08000000,
		0x00100032,0x00000001,0x00100046,0x00000005,0x00201046,0x00000000,0x00000000,0x08000000,
		0x00100032,0x00000004,0x00100ae6,0x00000005,0x00201046,0x00000000,0x00000000,0x05000036,
		0x00100042,0x00000001,0x00004001,0x3f800000,0x08000010,0x00100082,0x00000001,0x00100246,
		0x00000001,0x00208346,0x00000000,0x00000000,0x05000036,0x00102012,0x00000000,0x0010003a,
		0x00000001,0x08000010,0x00100082,0x00000001,0x00100246,0x00000001,0x00208346,0x00000000,
		0x00000001,0x05000036,0x00102022,0x00000000,0x0010003a,0x00000001,0x08000010,0x00100082,
		0x00000001,0x00100246,0x00000001,0x00208346,0x00000000,0x00000002,0x08000010,0x00100012,
		0x00000001,0x00100246,0x00000001,0x00208346,0x00000000,0x00000003,0x05000036,0x00102042,
		0x00000000,0x0010003a,0x00000001,0x05000036,0x00102082,0x00000000,0x0010000a,0x00000001,
		0x06000036,0x001020f2,0x00000001,0x00201e46,0x00000000,0x00000004,0x0a000000,0x001000f2,
		0x00000001,0x80201446,0x00000041,0x00000000,0x00000002,0x00201ee6,0x00000000,0x00000002,
		0x0b000032,0x00100062,0x00000002,0x00201106,0x00000000,0x00000005,0x00100106,0x00000001,
		0x00201106,0x00000000,0x00000002,0x08000038,0x001000f2,0x00000001,0x00100e46,0x00000001,
		0x00201e46,0x00000000,0x00000005,0x05000036,0x00102032,0x00000002,0x00100596,0x00000002,
		0x01000013,0x05000036,0x00100042,0x00000004,0x00004001,0x3f800000,0x08000010,0x00100022,
		0x00000002,0x00100246,0x00000004,0x00208346,0x00000000,0x00000000,0x05000036,0x00102012,
		0x00000000,0x0010001a,0x00000002,0x08000010,0x00100022,0x00000002,0x00100246,0x00000004,
		0x00208346,0x00000000,0x00000001,0x05000036,0x00102022,0x00000000,0x0010001a,0x00000002,
		0x08000010,0x00100022,0x00000002,0x00100246,0x00000004,0x00208346,0x00000000,0x00000002,
		0x08000010,0x00100042,0x00000002,0x00100246,0x00000004,0x00208346,0x00000000,0x00000003,
		0x05000036,0x00102042,0x00000000,0x0010001a,0x00000002,0x05000036,0x00102082,0x00000000,
		0x0010002a,0x00000002,0x06000036,0x001020f2,0x00000001,0x00201e46,0x00000000,0x00000004,
		0x0c000032,0x001000f2,0x00000004,0x00100ee6,0x00000001,0x00004002,0x3f800000,0x00000000,
		0x00000000,0x3f800000,0x00100446,0x00000001,0x07000000,0x00100032,0x00000001,0x00100046,
		0x00000001,0x00100ae6,0x00000001,0x08000000,0x00100032,0x00000001,0x00100046,0x00000001,
		0x00201046,0x00000000,0x00000002,0x08000000,0x001000f2,0x00000004,0x00100e46,0x00000004,
		0x00201446,0x00000000,0x00000002,0x05000036,0x00102032,0x00000002,0x00100046,0x00000004,
		0x01000013,0x07000038,0x001000f2,0x00000002,0x00100e46,0x00000000,0x00100006,0x00000002,
		0x0a000032,0x00100052,0x00000005,0x00100756,0x00000000,0x00100006,0x00000003,0x80100206,
		0x00000041,0x00000002,0x09000032,0x001000a2,0x00000005,0x00100806,0x00000000,0x00100006,
		0x00000003,0x00100d56,0x00000002,0x08000000,0x00100032,0x00000000,0x00100046,0x00000005,
		0x00201046,0x00000000,0x00000000,0x08000000,0x00100032,0x00000002,0x00100ae6,0x00000005,
		0x00201046,0x00000000,0x00000000,0x05000036,0x00100042,0x00000000,0x00004001,0x3f800000,
		0x08000010,0x00100082,0x00000000,0x00100246,0x00000000,0x00208346,0x00000000,0x00000000,
		0x05000036,0x00102012,0x00000000,0x0010003a,0x00000000,0x08000010,0x00100082,0x00000000,
		0x00100246,0x00000000,0x00208346,0x00000000,0x00000001,0x05000036,0x00102022,0x00000000,
		0x0010003a,0x00000000,0x08000010,0x00100082,0x00000000,0x00100246,0x00000000,0x00208346,
		0x00000000,0x00000002,0x08000010,0x00100012,0x00000000,0x00100246,0x00000000,0x00208346,
		0x00000000,0x00000003,0x05000036,0x00102042,0x00000000,0x0010003a,0x00000000,0x05000036,
		0x00102082,0x00000000,0x0010000a,0x00000000,0x06000036,0x001020f2,0x00000001,0x00201e46,
		0x00000000,0x00000004,0x05000036,0x00102032,0x00000002,0x00100ae6,0x00000004,0x01000013,
		0x05000036,0x00100042,0x00000002,0x00004001,0x3f800000,0x08000010,0x00100012,0x00000000,
		0x00100246,0x00000002,0x00208346,0x00000000,0x00000000,0x05000036,0x00102012,0x00000000,
		0x0010000a,0x00000000,0x08000010,0x00100012,0x00000000,0x00100246,0x00000002,0x00208346,
		0x00000000,0x00000001,0x05000036,0x00102022,0x00000000,0x0010000a,0x00000000,0x08000010,
		0x00100012,0x00000000,0x00100246,0x00000002,0x00208346,0x00000000,0x00000002,0x08000010,
		0x00100022,0x00000000,0x00100246,0x00000002,0x00208346,0x00000000,0x00000003,0x05000036,
		0x00102042,0x00000000,0x0010000a,0x00000000,0x05000036,0x00102082,0x00000000,0x0010001a,
		0x00000000,0x06000036,0x001020f2,0x00000001,0x00201e46,0x00000000,0x00000004,0x05000036,
		0x00102032,0x00000002,0x00100046,0x00000001,0x01000013,0x0100003e,0x54415453,0x00000074,
		0x00000049,0x00000006,0x00000000,0x0000000c,0x00000026,0x00000000,0x00000000,0x00000001,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000004,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000004,0x00000000,0x00000000,0x00000000,0x00000001,
		0x00000005,0x00000004,0x00000000,0x00000000,0x00000000
	};

	//////////////////////////////////////////////////////////////////////
	// Data for Sprite_PS

	uint32 WEAKSYM Sprite_PS_Data[] =
	{
		0x43425844,0x9bc5ee9f,0x4de0cf2d,0x23e15e1b,0xb5af7864,0x00000001,0x00000298,0x00000005,
		0x00000034,0x000000d8,0x0000014c,0x00000180,0x0000021c,0x46454452,0x0000009c,0x00000000,
		0x00000000,0x00000002,0x0000001c,0xffff0400,0x00008100,0x00000067,0x0000005c,0x00000003,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x00000062,0x00000002,
		0x00000005,0x00000004,0xffffffff,0x00000000,0x00000001,0x0000000c,0x6c706d73,0x61700072,
		0x4d006567,0x6f726369,0x74666f73,0x29522820,0x534c4820,0x6853204c,0x72656461,0x6d6f4320,
		0x656c6970,0x2e362072,0x36392e33,0x312e3030,0x34383336,0xababab00,0x4e475349,0x0000006c,
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

	extern ConstBufferOffset const WEAKSYM Sprite_GS_vConstants_Offsets[1] =
	{
		{ "TransformMatrix", 0 }
	};

	// no defaults for vConstants

	//////////////////////////////////////////////////////////////////////
	// const buffer names table

	extern char const WEAKSYM *Sprite_GS_ConstBufferNames[] =
	{
		"vConstants"
	};

	//////////////////////////////////////////////////////////////////////
	// Sampler names

	extern char const WEAKSYM * Sprite_PS_SamplerNames[] =
	{
		"smplr"
	};

	//////////////////////////////////////////////////////////////////////
	// Texture names

	extern char const WEAKSYM * Sprite_PS_TextureNames[] =
	{
		"page"
	};

	//////////////////////////////////////////////////////////////////////
	// Input Element descs

	extern D3D11_INPUT_ELEMENT_DESC const WEAKSYM Sprite_VS_InputElements[9] =
	{
		{ "float_Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "float_Pivot", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "float_Size", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "float_Scale", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "float_UVa", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "float_UVb", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "float_Rotation", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "byte_Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "norm8_Flip", 0, DXGI_FORMAT_R8G8B8A8_SNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//////////////////////////////////////////////////////////////////////
	// Binding runs

	extern Shader::BindingState WEAKSYM Sprite_Bindings[] = {
		{{}, {}, {}},
		{},
		{},
		{{}, {}, {{{0,1}}}},
		{{{{0,1}}}, {{{0,1}}}, {}},
		{}
	};

	//////////////////////////////////////////////////////////////////////
	// Shader struct

	struct Sprite : ShaderState
	{
		//////////////////////////////////////////////////////////////////////
		// InputVertex

		struct InputVertex
		{
			Float2 Position;
			Float2 Pivot;
			Float2 Size;
			Float2 Scale;
			Float2 UVa;
			Float2 UVb;
			Float1 Rotation;
			Byte4 Color;
			SByte4 Flip;
		};

		using VertBuffer = VertexBuffer<InputVertex>;

		//////////////////////////////////////////////////////////////////////
		// Vertex Shader

		struct VS : VertexShader
		{
			// Constructor

			VS()
				: VertexShader(0, null, 0, null, 0, null, null, null, Sprite_Bindings[ShaderType::Vertex])
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Geometry Shader

		struct GS : GeometryShader
		{
			// Const Buffers

			struct vConstants_t
			{
				Float4x4 TransformMatrix;
			};

			ConstBuffer<vConstants_t> vConstants;
			enum { vConstants_index = 0 };

			// Constructor

			GS()
				: GeometryShader(1, Sprite_GS_ConstBufferNames, 0, null, 0, null, null, null, Sprite_Bindings[ShaderType::Geometry])
				, vConstants(1, Sprite_GS_vConstants_Offsets, null, this, 0, 0)
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
				: PixelShader(0, null, 1, Sprite_PS_SamplerNames, 1, Sprite_PS_TextureNames, textures, samplers, Sprite_Bindings[ShaderType::Pixel])
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

		Sprite(): ShaderState(Sprite_BlendDesc, Sprite_DepthStencilDesc, Sprite_RasterizerDesc)
		{
			vs.Create(Sprite_VS_Data, 1360, Sprite_VS_InputElements, _countof(Sprite_VS_InputElements));
			gs.Create(Sprite_GS_Data, 2996);
			ps.Create(Sprite_PS_Data, 664);
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
