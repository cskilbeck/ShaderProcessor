//////////////////////////////////////////////////////////////////////

cbuffer VertConstants
{
	matrix ModelMatrix;
	matrix TransformMatrix;
}

//////////////////////////////////////////////////////////////////////

struct VS_INPUT
{
	float3 Position : float_Position;
	half2 TexCoord : half_TexCoord;
	float4 Color : byte_Color;
	float3 Normal : float_Normal;
};

//////////////////////////////////////////////////////////////////////

struct PS_INPUT
{
	float4 Position : SV_Position;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

//////////////////////////////////////////////////////////////////////

PS_INPUT vsMain(VS_INPUT v)
{
	PS_INPUT o;
	float4 pos = float4(v.Position.xyz, 1);
	o.Position = mul(pos, TransformMatrix);
	o.Normal = normalize(mul(v.Normal, (float3x3)ModelMatrix));
	o.TexCoord = float2(v.TexCoord.x, v.TexCoord.y);
	return o;
}

//////////////////////////////////////////////////////////////////////

cbuffer ColourStuff
{
	float3 LightDirection;
};

//////////////////////////////////////////////////////////////////////

sampler tex1Sampler;
Texture2D picTexture;

//////////////////////////////////////////////////////////////////////

float4 psMain(PS_INPUT i) : SV_TARGET
{
	float4 c = picTexture.Sample(tex1Sampler, i.TexCoord);
	float l = saturate(dot(normalize(i.Normal), LightDirection));
	return float4((l * c).xyz, c.w);
}
