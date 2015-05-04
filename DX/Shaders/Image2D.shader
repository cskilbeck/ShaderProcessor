//////////////////////////////////////////////////////////////////////

#pragma depth(disabled, write=disabled)
#pragma blend(0, enabled, src=src_alpha, dest=inv_src_alpha, op=add)
#pragma culling(mode=none)

//////////////////////////////////////////////////////////////////////

cbuffer vConstants
{
	matrix TransformMatrix;
}

cbuffer ClipPlanes : register(b4)
{
	float4 Clip0;
	float4 Clip1;
	float4 Clip2;
	float4 Clip3;
};

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
	float4 Clip : SV_ClipDistance0;
};

//////////////////////////////////////////////////////////////////////

PS_INPUT vsMain(VS_INPUT v)
{
	PS_INPUT o;
	o.Position = mul(float4(v.Position.xy, 0.5, 1), TransformMatrix);
	o.TexCoord = v.TexCoord;
	o.Color = v.Color;
	o.Clip = float4(dot(o.Position.xy, Clip0.xy) - Clip0.w,
					dot(o.Position.xy, Clip1.xy) - Clip1.w,
					dot(o.Position.xy, Clip2.xy) - Clip2.w,
					dot(o.Position.xy, Clip3.xy) - Clip3.w);
	return o;
}

//////////////////////////////////////////////////////////////////////

float4 psMain(PS_INPUT i) : SV_TARGET
{
	return page.Sample(smplr, i.TexCoord) * i.Color;
}
