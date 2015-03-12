cbuffer VertConstants
{
	matrix Transform;
};

struct VS_INPUT
{
	float3 position				: semantic : ();
	float3 instancePosition		: semantic : (stream = 1, instances = 1);
	float4 instanceColor		: semantic : (type = byte, stream = 1, instances = 1);
};

struct PS_INPUT
{
	float4 position			: SV_Position;
	float4 color			: COLOR;
};

PS_INPUT vsMain(VS_INPUT i)
{
	PS_INPUT o;
	float4 pos = float4(i.position + i.instancePosition, 1);
	o.position = mul(pos, Transform);
	o.color = i.instanceColor;
	return o;
}

float4 psMain(PS_INPUT i) : SV_Target
{
	return i.color;
}
