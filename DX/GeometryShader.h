//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct GeometryShader: Shader
	{
		//////////////////////////////////////////////////////////////////////

		GeometryShader(uint numConstBuffers, char const **constBufferNames,
					   uint numSamplers, char const **samplerNames,
					   uint numTextures, char const **textureNames,
					   Texture **textureArray,
					   Sampler **samplerArray)
		   : Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames, textureArray, samplerArray)
		{
		}

		//////////////////////////////////////////////////////////////////////

		void Activate(ID3D11DeviceContext *context)
		{
			ID3D11ShaderResourceView *srp[16];
			ID3D11SamplerState *ssp[16];

			uint resources = UpdateTextures(srp);
			uint samples = UpdateSamplers(ssp);

			context->GSSetShaderResources(0, resources, srp);
			context->GSSetSamplers(0, samples, ssp);

			uint c = 0;
			for(auto b : mBindingInfo.mBindRuns)
			{
				context->GSSetConstantBuffers(b.mBindPoint, b.mBindCount, &mBindingInfo.mPointers[c]);
				c += b.mBindCount;
			}

			context->GSSetShader(mGeometryShader, null, 0);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(void const *blob, size_t size)
		{
			DXR(Device->CreateGeometryShader(blob, size, null, &mGeometryShader));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Load(FileResource &f)
		{
			void const *p;
			size_t s;
			Shader::GetOffsetInSOBFile(f, ShaderType::Geometry, p, s);
			DXT(Create(p, s));
		}

		//////////////////////////////////////////////////////////////////////

		DXPtr<ID3D11GeometryShader> mGeometryShader;
	};
}
