//////////////////////////////////////////////////////////////////////

#pragma depth(disabled, write=disabled)
#pragma blend_0(enabled, src=src_alpha, dest=inv_src_alpha, op=add)

//////////////////////////////////////////////////////////////////////

cbuffer vConstants
{
	matrix TransformMatrix;
}

Texture2D page;
sampler smplr;

//////////////////////////////////////////////////////////////////////

struct VS_INPUT
{
	float2 Position : float_Position;
	float2 Size: half_Size;
	float2 UVa: half_UVa;
	float2 UVb: half_UVb;
	float4 Color: byte_Color;
};

//////////////////////////////////////////////////////////////////////

struct PS_INPUT
{
	float4 Position : SV_Position;
	float4 Color: COLOR;
	float2 UV: TEXCOORD0;
};

//////////////////////////////////////////////////////////////////////

VS_INPUT vsMain(VS_INPUT v)
{
	return v;
}

//////////////////////////////////////////////////////////////////////

[maxvertexcount(4)]
void gsMain(point VS_INPUT i[1], inout TriangleStream<PS_INPUT> stream)
{
	float2 c[4];
	float2 uv[4];

	c[0] = float2(0,0);
	c[1] = float2(i[0].Size.x, 0);
	c[2] = float2(0, i[0].Size.y);
	c[3] = i[0].Size;

	uv[0] = i[0].UVa;
	uv[1] = float2(i[0].UVb.x, i[0].UVa.y);
	uv[2] = float2(i[0].UVa.x, i[0].UVb.y);
	uv[3] = i[0].UVb;

	stream.RestartStrip();
	for(int j=0; j<4; ++j)
	{
		PS_INPUT o;
		o.Position = mul(float4(c[j] + i[0].Position, 0.5, 1), TransformMatrix);
		o.Color = i[0].Color;
		o.UV = uv[j];
		stream.Append(o);
	}
}

//////////////////////////////////////////////////////////////////////

float4 psMain(PS_INPUT i) : SV_TARGET
{
	float4 c = page.Sample(smplr, i.UV);
	return c * i.Color;
}
