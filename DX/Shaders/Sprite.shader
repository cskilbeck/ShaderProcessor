//////////////////////////////////////////////////////////////////////

#pragma depth(disabled, write=disabled)
#pragma blend(0, enabled, src=src_alpha, dest=inv_src_alpha, op=add)

//////////////////////////////////////////////////////////////////////

cbuffer vConstants
{
	matrix TransformMatrix;
}

//////////////////////////////////////////////////////////////////////

Texture2D page;
sampler smplr;

//////////////////////////////////////////////////////////////////////

struct VS_INPUT
{
	float2 Position	: semantic : ();
	float2 Pivot	: semantic : ();
	float2 Size		: semantic : ();
	float2 Scale	: semantic : ();
	float2 UVa		: semantic : ();
	float2 UVb		: semantic : ();
	float Rotation	: semantic : ();
	float4 Color	: semantic : (type=byte);
	float4 Flip		: semantic : (type=norm8);
};

//////////////////////////////////////////////////////////////////////

struct PS_INPUT
{
	float4 Position : SV_Position;
	float4 Color: COLOR0;
	float2 UV: TEXCOORD0;
};

//////////////////////////////////////////////////////////////////////

VS_INPUT vsMain(VS_INPUT v)
{
	return v;
}

//////////////////////////////////////////////////////////////////////

float2 rotate(float2 p, float s, float c)
{
	return float2(p.x * c - p.y * s, p.x * s + p.y * c);
}

//////////////////////////////////////////////////////////////////////

[maxvertexcount(4)]
void gsMain(point VS_INPUT i[1], inout TriangleStream<PS_INPUT> stream)
{
	float sin, cos;
	sincos(i[0].Rotation, sin, cos);

	float2 size = i[0].Size;
	float2 pivot = i[0].Pivot * size;

	float2 uvs = i[0].UVb - i[0].UVa;
	float2 uvoff = i[0].Flip.xy * uvs;
	float2 uvscale = i[0].Flip.zw * uvs;

	float2 c[4];
	float2 uv[4];

	c[0] = -pivot;
	c[3] = size - pivot;
	c[1] = float2(c[3].x, c[0].y);
	c[2] = float2(c[0].x, c[3].y);

	uv[0] = float2(0, 0);
	uv[1] = float2(1, 0);
	uv[2] = float2(0, 1);
	uv[3] = float2(1, 1);

	stream.RestartStrip();
	for(int j=0; j<4; ++j)
	{
		PS_INPUT o;
		o.Position = mul(float4(rotate(c[j] * i[0].Scale, sin, cos) + i[0].Position, 0, 1), TransformMatrix);
		o.Color = i[0].Color;
		o.UV = uv[j] * uvscale + uvoff + i[0].UVa;
		stream.Append(o);
	}
}

//////////////////////////////////////////////////////////////////////

float4 psMain(PS_INPUT i) : SV_TARGET
{
	float4 c = page.Sample(smplr, i.UV);
	return c * i.Color;
}
