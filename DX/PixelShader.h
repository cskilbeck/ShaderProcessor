//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct PixelShader: Shader
	{
		PixelShader(void const *blob, size_t size,
					uint numConstBuffers, char const **constBufferNames,
					uint numSamplers, char const **samplerNames,
					uint numTextures, char const **textureNames,
					Texture **textureArray,
					Sampler **samplerArray)
			: Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames, textureArray, samplerArray)
		{
			DXT(Device->CreatePixelShader(blob, size, null, &mPixelShader));
		}

		void Activate(ID3D11DeviceContext *context)
		{
			UpdateTextures();
			UpdateSamplers();

			context->PSSetShaderResources(0, mNumTextures, mTexturePointers.data());
			context->PSSetSamplers(0, mNumSamplers, mSamplerPointers.data());
			context->PSSetConstantBuffers(0, mNumConstBuffers, mConstBufferPointers.data());
			context->PSSetShader(mPixelShader, null, 0);
		}

		DXPtr<ID3D11PixelShader>			mPixelShader;
	};

}