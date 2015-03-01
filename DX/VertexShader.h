//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct VertexShader: Shader
	{
		//////////////////////////////////////////////////////////////////////

		VertexShader(uint numConstBuffers, char const **constBufferNames,
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

			uint c = 0;
			for(auto b : mBindingInfo.mBindRuns)
			{
				context->VSSetConstantBuffers(b.mBindPoint, b.mBindCount, &mBindingInfo.mPointers[c]);
				c += b.mBindCount;
			}

			context->VSSetShaderResources(0, resources, srp);
			context->VSSetSamplers(0, samples, ssp);
			context->IASetInputLayout(mInputLayout);
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

		HRESULT Load(FileResource &f, D3D11_INPUT_ELEMENT_DESC const *inputElements, uint inputElementCount)
		{
			void const *p;
			size_t s;
			Shader::GetOffsetInSOBFile(f, ShaderType::Vertex, p, s);
			DXR(Create(p, s, inputElements, inputElementCount));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		DXPtr<ID3D11VertexShader>			mVertexShader;
		DXPtr<ID3D11InputLayout>			mInputLayout;
	};
}