cbuffer VertConstants
{
	matrix Transform;
};

struct VS_INPUT
{
	float3 position				: semantic(name = mPosition, stream = 0);
	matrix instanceTransform	: semantic(name = mTransform, stream = 1, instances = 1);
	float4 instanceColor		: semantic(name = mColor, type = byte, stream = 2, instances = 2);
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
