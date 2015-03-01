cbuffer VertConstants : register(b6)
{
	matrix Transform;
};

struct VS_INPUT
{
	float3 position			: float_Position;
	matrix transform		: float_Matrix;
	float4 instanceColor	: byte_Color;
};

struct PS_INPUT
{
	float4 position: SV_Position;
	float4 color: COLOR;
};

PS_INPUT vsMain(VS_INPUT i)
{
	PS_INPUT o;
	o.position = mul(float4(i.position, 1), Transform);
	o.color = i.instanceColor;
	return o;
}

float4 psMain(PS_INPUT i) : SV_Target
{
	return i.color;
}