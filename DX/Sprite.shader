//////////////////////////////////////////////////////////////////////

cbuffer vConstants
{
	matrix TransformMatrix;
}

//////////////////////////////////////////////////////////////////////

struct VS_INPUT
{
	float2 Position : float_Position;	// in pixels
	float2 Size: float_Size;			// in pixels
	float4 Color: byte_Color;
};

//////////////////////////////////////////////////////////////////////

struct PS_INPUT
{
	float4 Position : SV_Position;
	float4 Color: COLOR;
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
	VS_INPUT v = i[0];
	float2 pos = v.Position;
	float2 br = pos + i[0].Size;
	float4 p[4];
	p[0] = float4(pos, 0.5, 1);
	p[1] = float4(br.x, pos.y, 0.5, 1);
	p[2] = float4(pos.x, br.y, 0.5, 1);
	p[3] = float4(br.x, br.y, 0.5, 1);
	stream.RestartStrip();
	for(int i=0; i<4; ++i)
	{
		PS_INPUT o;
		o.Position = mul(p[i], TransformMatrix);
		o.Color = v.Color;
		stream.Append(o);
	}
}

//////////////////////////////////////////////////////////////////////

float4 psMain(PS_INPUT i) : SV_TARGET
{
	return i.Color;
}
