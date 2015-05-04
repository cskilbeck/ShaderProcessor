//////////////////////////////////////////////////////////////////////

#pragma depth(disabled, write=disabled)
#pragma blend(0, enabled, src=src_alpha, dest=inv_src_alpha, op=add)

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

Texture2D page;
sampler smplr;

//////////////////////////////////////////////////////////////////////

struct VS_INPUT
{
	float2 Position	: semantic : ();
	float2 UV		: semantic : (type=half);
	float4 Color	: semantic : (type=byte);
};

//////////////////////////////////////////////////////////////////////

struct PS_INPUT
{
	float4 Position : SV_Position;
	float4 Color: COLOR;
	float2 UV: TEXCOORD0;
	float4 Clip : SV_ClipDistance0;
};

//////////////////////////////////////////////////////////////////////

PS_INPUT vsMain(VS_INPUT i)
{
	PS_INPUT o;
	o.Position = mul(float4(i.Position, 0, 1), TransformMatrix);
	o.Color = i.Color;
	o.UV = i.UV;
	o.Clip = float4(dot(o.Position.xy, Clip0.xy) - Clip0.w,
					dot(o.Position.xy, Clip1.xy) - Clip1.w,
					dot(o.Position.xy, Clip2.xy) - Clip2.w,
					dot(o.Position.xy, Clip3.xy) - Clip3.w);
	return o;
}

//////////////////////////////////////////////////////////////////////

float4 psMain(PS_INPUT i) : SV_TARGET
{
	float4 c = page.Sample(smplr, i.UV);
	return c * i.Color;
}
