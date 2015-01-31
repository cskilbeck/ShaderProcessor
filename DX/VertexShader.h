//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

template<D3D11_INPUT_ELEMENT_DESC const *inputElements, uint inputElementCount> struct VertexShader: Shader
{
	VertexShader(void const *blob, size_t size, uint numConstBuffers, char const **constBufferNames, uint numSamplers, char const **samplerNames, uint numTextures, char const **textureNames)
		: Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames)
	{
		DXT(gDevice->CreateVertexShader(blob, size, null, &mVertexShader));
		DXT(gDevice->CreateInputLayout(inputElements, inputElementCount, blob, size, &mInputLayout));
	}

	void Activate(ID3D11DeviceContext *context)
	{
		context->IASetInputLayout(mInputLayout);
		context->VSSetConstantBuffers(0, (uint)mConstBufferPointers.size(), mConstBufferPointers.empty() ? null : mConstBufferPointers.data());	// TODO: only changed ones?
		context->VSSetShader(mVertexShader, null, 0);
	}

	DXPtr<ID3D11VertexShader>			mVertexShader;
	DXPtr<ID3D11InputLayout>			mInputLayout;
};

