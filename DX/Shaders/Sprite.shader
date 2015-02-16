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
	float2 Pivot: float_Pivot;
	float2 Size: float_Size;
	float2 Scale: float_Scale;
	float2 UVa: float_UVa;
	float2 UVb: float_UVb;
	float Rotation: float_Rotation;
	float4 Color: byte_Color;
	float4 UVFlip: norm8_Flip;	// X = UVOffsetX, Y = UVOffsetY, Z = UVScaleX, W = UVScaleY
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
	float2 uvoff = i[0].UVFlip.xy * uvs;
	float2 uvscale = i[0].UVFlip.zw * uvs;

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
		o.Position = mul(float4(rotate(c[j] * i[0].Scale, sin, cos) + i[0].Position, 0.5, 1), TransformMatrix);
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
