
#if !defined(_DEBUG)
#define BOB def
#endif

struct ABC
{ 
	bool BOB;
};

struct Cheese
{
	float2 Toss;
};

cbuffer VertConstants : register(b6)
{
	struct bob {
		ABC ghi;
		float Baz;
	} Bill;

	Cheese choose;
	matrix Transform;
};

struct Foo
{
	matrix transform : float_Matrix;
};

struct VS_INPUT
{
	float3 position			: float_Position;
	Foo foo;
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
	o.position = mul(float4(i.position, 1), Transform) + Bill.Baz;
	o.color = i.instanceColor;
	return o;
}

float4 psMain(PS_INPUT i) : SV_Target
{
	return i.color;
}