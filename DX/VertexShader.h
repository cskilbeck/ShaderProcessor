//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{

	template<D3D11_INPUT_ELEMENT_DESC const *inputElements, uint inputElementCount> struct VertexShader: Shader
	{
		VertexShader(void const *blob, size_t size, uint numConstBuffers, char const **constBufferNames, uint numSamplers, char const **samplerNames, uint numTextures, char const **textureNames, Texture **textureArray, Sampler **samplerArray)
			: Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames, textureArray, samplerArray)
		{
			DXT(DX::Device->CreateVertexShader(blob, size, null, &mVertexShader));
			DXT(DX::Device->CreateInputLayout(inputElements, inputElementCount, blob, size, &mInputLayout));
		}

		void Activate(ID3D11DeviceContext *context)
		{
			UpdateTextures();
			UpdateSamplers();

			context->IASetInputLayout(mInputLayout);
			context->VSSetShaderResources(0, mNumTextures, mTexturePointers.data());
			context->VSSetSamplers(0, mNumSamplers, mSamplerPointers.data());
			context->VSSetConstantBuffers(0, mNumConstBuffers, mConstBufferPointers.data());
			context->VSSetShader(mVertexShader, null, 0);
		}

		DXPtr<ID3D11VertexShader>			mVertexShader;
		DXPtr<ID3D11InputLayout>			mInputLayout;
	};

}