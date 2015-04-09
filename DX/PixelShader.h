//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct PixelShader: Shader
	{
		//////////////////////////////////////////////////////////////////////

		PixelShader(uint32 numConstBuffers, char const **constBufferNames,
					uint32 numSamplers, char const **samplerNames,
					uint32 numTextures, char const **textureNames,
					Texture **textureArray,
					Sampler **samplerArray,
					BindingState &bindingState)
			: Shader(numConstBuffers, constBufferNames,
						numSamplers, samplerNames,
						numTextures, textureNames,
						textureArray,
						samplerArray,
						bindingState)
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

		HRESULT D3DCreate(void const *blob, size_t size) override
		{
			DXT(Device->CreatePixelShader(blob, size, null, &mPixelShader));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(Resource &f)
		{
			DXR(Shader::Create(FindShaderInSOBFile(f, ShaderType::Pixel)));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		DXPtr<ID3D11PixelShader> mPixelShader;
	};
}
