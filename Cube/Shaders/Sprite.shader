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

	float2 pivot = i[0].Pivot * i[0].Size;

	float2 c[4];
	float2 uv[4];

	c[0] = -pivot;
	c[3] = i[0].Size - pivot;
	c[1] = float2(c[3].x, c[0].y);
	c[2] = float2(c[0].x, c[3].y);

	uv[0] = i[0].UVa;
	uv[1] = float2(i[0].UVb.x, i[0].UVa.y);
	uv[2] = float2(i[0].UVa.x, i[0].UVb.y);
	uv[3] = i[0].UVb;

	stream.RestartStrip();
	for(int j=0; j<4; ++j)
	{
		PS_INPUT o;
		o.Position = mul(float4(rotate(c[j] * i[0].Scale, sin, cos) + i[0].Position, 0.5, 1), TransformMatrix);
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
