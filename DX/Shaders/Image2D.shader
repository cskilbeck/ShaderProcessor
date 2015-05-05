//////////////////////////////////////////////////////////////////////

#pragma blend(0, enabled, src=src_alpha, dest=inv_src_alpha, op=add)
#pragma depth(disabled, write=disabled)
#pragma culling(mode=none)

//////////////////////////////////////////////////////////////////////

#include "2D.h"

//////////////////////////////////////////////////////////////////////

sampler smplr;
Texture2D page;

//////////////////////////////////////////////////////////////////////

struct VS_INPUT
{
	float2 Position : semantic : ();
	float2 TexCoord : semantic : (type = half);
	float4 Color	: semantic : (type = byte);
};

//////////////////////////////////////////////////////////////////////

struct PS_INPUT
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
	float4 Color	: COLOR0;
	float4 Clip		: SV_ClipDistance0;
};

//////////////////////////////////////////////////////////////////////

PS_INPUT vsMain(VS_INPUT v)
{
	PS_INPUT o;
	o.Position = mul(float4(v.Position.xy, 0.5, 1), TransformMatrix);
	o.TexCoord = v.TexCoord;
	o.Color = v.Color;
	o.Clip = DoClip(o.Position.xy);
	return o;
}

//////////////////////////////////////////////////////////////////////

float4 psMain(PS_INPUT i) : SV_TARGET
{
	return page.Sample(smplr, i.TexCoord) * i.Color;
}
