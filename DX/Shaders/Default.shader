//////////////////////////////////////////////////////////////////////

cbuffer VertConstants
{
	matrix ModelMatrix;
	matrix TransformMatrix;
}

//////////////////////////////////////////////////////////////////////

struct VS_INPUT
{
	float3 Position	: semantic : ();
	float2 TexCoord	: semantic : (type=half);
	float4 Color	: semantic : (type=byte);
	float3 Normal	: semantic : ();
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

cbuffer Camera : register(b3)
{
	float3 cameraPos;
}

//////////////////////////////////////////////////////////////////////

cbuffer Light : register(b4)
{
	float3 lightPos;
	float3 ambientColor;
	float3 diffuseColor;
	float3 specColor;
};

//////////////////////////////////////////////////////////////////////

sampler tex1Sampler;
Texture2D picTexture;

//////////////////////////////////////////////////////////////////////

float4 psMain(PS_INPUT i) : SV_TARGET
{
	float4 c = picTexture.Sample(tex1Sampler, i.TexCoord);
	float3 lightDir = normalize(lightPos - i.WorldPos);
	float3 viewDir = normalize(cameraPos - i.WorldPos);
	float3 halfDir = normalize(lightDir + viewDir);
	float diffuse = max(dot(lightDir, i.Normal), 0);
	float specAngle = max(dot(halfDir, i.Normal), 0);
	float specular = pow(specAngle, 190);
	return float4((ambientColor + diffuseColor * diffuse) * c.xyz + specular * specColor, c.w);
}
