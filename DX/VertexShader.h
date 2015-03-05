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
					 Sampler **samplerArray,
					 BindingState &bindState)
			 : Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames, textureArray, samplerArray, bindState)
		{
		}

		//////////////////////////////////////////////////////////////////////

		void Activate(ID3D11DeviceContext *context)
		{
			Set<&ID3D11DeviceContext::VSSetConstantBuffers,
				&ID3D11DeviceContext::VSSetSamplers,
				&ID3D11DeviceContext::VSSetShaderResources>(context);
			context->IASetInputLayout(mInputLayout);
			context->VSSetShader(mVertexShader, null, 0);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT CreateInputLayout(void const *blob, size_t size, D3D11_INPUT_ELEMENT_DESC const *inputElements, uint inputElementCount)
		{
			DXR(Device->CreateInputLayout(inputElements, inputElementCount, blob, size, &mInputLayout));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(void const *blob, size_t size) override
		{
			DXR(Device->CreateVertexShader(blob, size, null, &mVertexShader));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(void const *data, size_t size, D3D11_INPUT_ELEMENT_DESC const *inputElements, uint inputElementCount)
		{
			DXR(Create(data, size));
			DXR(CreateInputLayout(data, size, inputElements, inputElementCount));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Load(FileResource &f, D3D11_INPUT_ELEMENT_DESC const *inputElements, uint inputElementCount)
		{
			Resource r = FindShaderInSOBFile(f, ShaderType::Vertex);
			DXR(Shader::Create(r));
			DXR(CreateInputLayout(r.Data(), r.Size(), inputElements, inputElementCount));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		DXPtr<ID3D11VertexShader>			mVertexShader;
		DXPtr<ID3D11InputLayout>			mInputLayout;
	};
}