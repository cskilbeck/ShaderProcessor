//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct Viewport
{
	Viewport(float left, float top, float width, float height, float minZ, float maxZ)
	{
		vp.TopLeftX = left;
		vp.TopLeftY = top;
		vp.Width = width;
		vp.Height = height;
		vp.MinDepth = minZ;
		vp.MaxDepth = maxZ;
	}

	void Activate(ID3D11DeviceContext *context)
	{
		context->RSSetViewports(1, &vp);
	}

	D3D11_VIEWPORT vp;
};