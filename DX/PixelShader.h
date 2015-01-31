//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct PixelShader: Shader
{
	PixelShader(void const *blob, size_t size, uint numConstBuffers, char const **constBufferNames, uint numSamplers, char const **samplerNames, uint numTextures, char const **textureNames)
		: Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames)
	{
		DXT(gDevice->CreatePixelShader(blob, size, null, &mPixelShader));
	}

	void Activate(ID3D11DeviceContext *context)
	{
		context->PSSetShaderResources(0, (uint)mTexturePointers.size(), mTexturePointers.empty() ? null : mTexturePointers.data());
		context->PSSetSamplers(0, (uint)mSamplers.size(), mSamplerPointers.empty() ? null : mSamplerPointers.data());
		context->PSSetConstantBuffers(0, (uint)mConstBufferPointers.size(), mConstBufferPointers.empty() ? null : mConstBufferPointers.data());
		context->PSSetShader(mPixelShader, null, 0);
	}

	DXPtr<ID3D11PixelShader>			mPixelShader;
};

