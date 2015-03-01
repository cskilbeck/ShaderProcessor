//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct PixelShader: Shader
	{
		//////////////////////////////////////////////////////////////////////

		PixelShader(uint numConstBuffers, char const **constBufferNames,
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

			context->PSSetShaderResources(0, resources, srp);
			context->PSSetSamplers(0, samples, ssp);

			uint c = 0;
			for(auto b : mBindingInfo.mBindRuns)
			{
				TRACE("PS Binding: %d\n", b.mBindCount);
				for(uint i = 0; i < b.mBindCount; ++i)
				{
					TRACE("BindPoint %d: %d = %p\n", b.mBindPoint + i, c + i, mBindingInfo.mPointers[c + i]);
				}

				context->PSSetConstantBuffers(b.mBindPoint, b.mBindCount, &mBindingInfo.mPointers[c]);
				c += b.mBindCount;
			}

			context->PSSetShader(mPixelShader, null, 0);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(void const *blob, size_t size)
		{
			DXT(Device->CreatePixelShader(blob, size, null, &mPixelShader));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Load(FileResource &f)
		{
			void const *p;
			size_t s;
			Shader::GetOffsetInSOBFile(f, ShaderType::Pixel, p, s);
			DXT(Create(p, s));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		DXPtr<ID3D11PixelShader>			mPixelShader;
	};

}