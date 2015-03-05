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
	float2 TexCoord : half_TexCoord;
	float4 Color : byte_Color;
	float3 Normal : float_Normal;
};

//////////////////////////////////////////////////////////////////////

struct PS_INPUT
{
	float4 Position : SV_Position;
	float3 WorldPos : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float2 TexCoord : TEXCOORD2;
};

//////////////////////////////////////////////////////////////////////

PS_INPUT vsMain(VS_INPUT v)
{
	PS_INPUT o;
	float4 pos = float4(v.Position, 1);
	o.Position = mul(pos, TransformMatrix);
	o.WorldPos = mul(pos, ModelMatrix).xyz;
	o.Normal = normalize(mul(v.Normal, (float3x3)ModelMatrix));
	o.TexCoord = v.TexCoord;
	return o;
}

//////////////////////////////////////////////////////////////////////

cbuffer Camera : register(b2)
{
	float3 cameraPos;
}

//////////////////////////////////////////////////////////////////////

cbuffer Light : register(b1)
{
	float3 lightPos;
	float3 ambientColor;
	float3 diffuseColor;
	float3 specColor;
};

//////////////////////////////////////////////////////////////////////

Texture2D picTexture : register(t1);
sampler tex1Sampler;

//////////////////////////////////////////////////////////////////////

float4 psMain(PS_INPUT i) : SV_TARGET
{
	float4 c = picTexture.Sample(tex1Sampler, i.TexCoord);
	float3 lightDir = normalize(lightPos - i.WorldPos);
	float3 viewDir = normalize(cameraPos - i.WorldPos);
	float3 halfDir = normalize(lightDir + viewDir);
	float diffuse = max(dot(lightDir, i.Normal), 0);
	float specAngle = max(dot(halfDir, i.Normal), 0);
	float specular = pow(specAngle, 1024);
	return float4((ambientColor + diffuseColor * diffuse) * c.xyz + specular * specColor, c.w);
}
