//////////////////////////////////////////////////////////////////////

cbuffer g_VertConstants2D : register(b0)
{
	matrix TransformMatrix;		// just an ortho projection to make pixels 1:1
}

cbuffer g_ClipPlanes2D : register(b4)
{
	float4 Clip0 = float4(0, 0, 0, 0);
	float4 Clip1 = float4(0, 0, 0, 0);
	float4 Clip2 = float4(0, 0, 0, 0);
	float4 Clip3 = float4(0, 0, 0, 0);
};

float4 DoClip(float2 position)
{
	return float4( dot(position, Clip0.xy) - Clip0.w,
				   dot(position, Clip1.xy) - Clip1.w,
				   dot(position, Clip2.xy) - Clip2.w,
				   dot(position, Clip3.xy) - Clip3.w);
}