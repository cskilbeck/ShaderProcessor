//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct PixelShader: Shader
	{
		//////////////////////////////////////////////////////////////////////

		PixelShader(tchar const *filename,
					uint numConstBuffers, char const **constBufferNames,
					uint numSamplers, char const **samplerNames,
					uint numTextures, char const **textureNames,
					Texture **textureArray,
					Sampler **samplerArray)
			: Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames, textureArray, samplerArray)
		{
			FileResource f(filename);
			if(!f.IsValid())
			{
				throw("Shader file not found");
			}
			DXT(Create(f.Data(), f.Size()));
		}

		//////////////////////////////////////////////////////////////////////

		PixelShader(void const *blob, size_t size,
					uint numConstBuffers, char const **constBufferNames,
					uint numSamplers, char const **samplerNames,
					uint numTextures, char const **textureNames,
					Texture **textureArray,
					Sampler **samplerArray)
			: Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames, textureArray, samplerArray)
		{
			DXT(Create(blob, size));
		}

		//////////////////////////////////////////////////////////////////////

		void Activate(ID3D11DeviceContext *context)
		{
			UpdateTextures();
			UpdateSamplers();

			context->PSSetShaderResources(0, mNumTextures, mTexturePointers.data());
			context->PSSetSamplers(0, mNumSamplers, mSamplerPointers.data());
			context->PSSetConstantBuffers(0, mNumConstBuffers, mConstBufferPointers.data());
			context->PSSetShader(mPixelShader, null, 0);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(void const *blob, size_t size)
		{
			DXT(Device->CreatePixelShader(blob, size, null, &mPixelShader));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		DXPtr<ID3D11PixelShader>			mPixelShader;
	};

}