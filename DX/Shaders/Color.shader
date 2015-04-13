//////////////////////////////////////////////////////////////////////

#pragma blend(0, enabled, src=src_alpha, dest=inv_src_alpha, op=add)
#pragma depth(enabled, write=disabled, comparison_func=less_equal)

//////////////////////////////////////////////////////////////////////

cbuffer VertConstants
{
	matrix TransformMatrix;
}

//////////////////////////////////////////////////////////////////////

struct VS_INPUT
{
	float3 Position : semantic : ();
	float4 Color	: semantic : (type = byte);
};

//////////////////////////////////////////////////////////////////////

struct PS_INPUT
{
	float4 Position : SV_Position;
	float4 Color : COLOR0;
};

//////////////////////////////////////////////////////////////////////

PS_INPUT vsMain(VS_INPUT v)
{
	PS_INPUT o;
	o.Position = mul(float4(v.Position, 1), TransformMatrix);
	o.Color = v.Color;
	return o;
}

//////////////////////////////////////////////////////////////////////

float4 psMain(PS_INPUT i) : SV_TARGET
{
	return i.Color;
}
