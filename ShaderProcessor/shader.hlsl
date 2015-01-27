//////////////////////////////////////////////////////////////////////

cbuffer VertConstants
{
	float4x4 ProjectionMatrix;
	float bob = 2.0;
}

//////////////////////////////////////////////////////////////////////

struct VS_INPUT
{
	float2 Pos : FLOAT_Position;
	float2 UV : FLOAT_TexCoord;
	float4 Color : BYTE_Color;
};

//////////////////////////////////////////////////////////////////////

struct PS_INPUT
{
	float4 Pos : SV_Position;
	float2 UV : TEXCOORD;
	float4 col : COLOR0;
};

//////////////////////////////////////////////////////////////////////

PS_INPUT vsMain(VS_INPUT input)
{
	PS_INPUT o;
	o.Pos = mul(float4(input.Pos.x, input.Pos.y, 0.5, 1.0), ProjectionMatrix);
	o.col = input.Color;
	return o;
}

sampler tex1Sampler;
Texture2D picTexture;

float4 tint = { 1.0f, 0.0f, 1.0f, 1.0f };

//////////////////////////////////////////////////////////////////////

float4 psMain(PS_INPUT input) : SV_TARGET
{
	float4 pixel = picTexture.Sample(tex1Sampler, input.UV);
	return input.col * pixel + tint;
}

