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
					Sampler **samplerArray,
					BindingState &bindingState)
			: Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames, textureArray, samplerArray, bindingState)
		{
		}

		//////////////////////////////////////////////////////////////////////

		void Activate(ID3D11DeviceContext *context)
		{
			Set<&ID3D11DeviceContext::PSSetConstantBuffers,
				&ID3D11DeviceContext::PSSetSamplers,
				&ID3D11DeviceContext::PSSetShaderResources>(context);
			context->PSSetShader(mPixelShader, null, 0);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(void const *blob, size_t size) override
		{
			DXT(Device->CreatePixelShader(blob, size, null, &mPixelShader));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Load(FileResource &f)
		{
			DXR(Shader::Create(FindShaderInSOBFile(f, ShaderType::Pixel)));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		DXPtr<ID3D11PixelShader> mPixelShader;
	};
}
