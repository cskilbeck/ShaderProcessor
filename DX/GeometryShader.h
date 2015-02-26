//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct GeometryShader: Shader
	{
		//////////////////////////////////////////////////////////////////////

		GeometryShader(tchar const *filename,
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

		GeometryShader(void const *blob, size_t size,
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
			context->GSSetShaderResources(0, mNumTextures, mTexturePointers.data());
			context->GSSetSamplers(0, mNumSamplers, mSamplerPointers.data());
			context->GSSetConstantBuffers(0, mNumConstBuffers, mConstBufferPointers.data());
			context->GSSetShader(mGeometryShader, null, 0);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(void const *blob, size_t size)
		{
			DXR(Device->CreateGeometryShader(blob, size, null, &mGeometryShader));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		DXPtr<ID3D11GeometryShader> mGeometryShader;
	};
}
