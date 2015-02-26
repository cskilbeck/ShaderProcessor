//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct VertexShader: Shader
	{
		//////////////////////////////////////////////////////////////////////

		VertexShader(tchar const *filename,
					 uint numConstBuffers, char const **constBufferNames,
					 uint numSamplers, char const **samplerNames,
					 uint numTextures, char const **textureNames,
					 Texture **textureArray,
					 Sampler **samplerArray,
					 D3D11_INPUT_ELEMENT_DESC const *inputElements, uint inputElementCount)
			 : Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames, textureArray, samplerArray)
		{
			FileResource f(filename);
			if(!f.IsValid())
			{
				throw("Shader file not found");
			}
			DXT(Create(f.Data(), f.Size(), inputElements, inputElementCount));
		}

		//////////////////////////////////////////////////////////////////////

		VertexShader(void const *blob, size_t size,
					 uint numConstBuffers, char const **constBufferNames,
					 uint numSamplers, char const **samplerNames,
					 uint numTextures, char const **textureNames,
					 Texture **textureArray,
					 Sampler **samplerArray,
					 D3D11_INPUT_ELEMENT_DESC const *inputElements, uint inputElementCount)
			: Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames, textureArray, samplerArray)
		{
			DXT(Create(blob, size, inputElements, inputElementCount));
		}

		//////////////////////////////////////////////////////////////////////

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

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(void const *blob, size_t size, D3D11_INPUT_ELEMENT_DESC const *inputElements, uint inputElementCount)
		{
			DXR(Device->CreateVertexShader(blob, size, null, &mVertexShader));
			DXR(Device->CreateInputLayout(inputElements, inputElementCount, blob, size, &mInputLayout));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		DXPtr<ID3D11VertexShader>			mVertexShader;
		DXPtr<ID3D11InputLayout>			mInputLayout;
	};
}