cbuffer VertConstants
{
	matrix Transform;
};

struct VS_INPUT
{
	float3 position:float_A;
	matrix instanceTransform:float_B;
	float4 instanceColor:byte_C;
};

struct PS_INPUT
{
	float4 position			: SV_Position;
	float4 color			: COLOR;
};

PS_INPUT vsMain(VS_INPUT i)
{
	PS_INPUT o;
	o.position = mul(mul(i.position, i.instanceTransform), Transform);
	o.color = i.instanceColor;
	return o;
}

float4 psMain(PS_INPUT i) : SV_Target
{
	return i.color;
}