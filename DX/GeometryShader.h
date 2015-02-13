//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct GeometryShader: Shader
	{
		GeometryShader(void const *blob, size_t size,
						uint numConstBuffers, char const **constBufferNames,
						uint numSamplers, char const **samplerNames,
						uint numTextures, char const **textureNames,
						Texture **textureArray,
						Sampler **samplerArray)
			: Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames, textureArray, samplerArray)
		{
			DXT(Device->CreateGeometryShader(blob, size, null, &mGeometryShader));
		}

		void Activate(ID3D11DeviceContext *context)
		{
			UpdateTextures();
			UpdateSamplers();

			context->GSSetShaderResources(0, mNumTextures, mTexturePointers.data());
			context->GSSetSamplers(0, mNumSamplers, mSamplerPointers.data());
			context->GSSetConstantBuffers(0, mNumConstBuffers, mConstBufferPointers.data());
			context->GSSetShader(mGeometryShader, null, 0);
		}

		DXPtr<ID3D11GeometryShader> mGeometryShader;
	};
}