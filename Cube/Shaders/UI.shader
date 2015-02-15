//////////////////////////////////////////////////////////////////////

cbuffer vConstants
{
	matrix TransformMatrix;
}

//////////////////////////////////////////////////////////////////////

cbuffer pConstants
{
	float4 Color;
}

//////////////////////////////////////////////////////////////////////

sampler smplr;
Texture2D page;

//////////////////////////////////////////////////////////////////////

struct VS_INPUT
{
	float2 Position : float_Position;
	float2 TexCoord : half_TexCoord;
};

//////////////////////////////////////////////////////////////////////

struct PS_INPUT
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD2;
};

//////////////////////////////////////////////////////////////////////

PS_INPUT vsMain(VS_INPUT v)
{
	PS_INPUT o;
	o.Position = mul(float4(v.Position.xy, 0.5, 1), TransformMatrix);
	o.TexCoord = v.TexCoord;
	return o;
}

//////////////////////////////////////////////////////////////////////

float4 psMain(PS_INPUT i) : SV_TARGET
{
	return page.Sample(smplr, i.TexCoord) * Color;
}
