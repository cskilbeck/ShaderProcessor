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
					   Sampler **samplerArray,
					   BindingState &bindingState)
			: Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames, textureArray, samplerArray, bindingState)
		{
		}

		//////////////////////////////////////////////////////////////////////

		void Activate(ID3D11DeviceContext *context)
		{
			uint s = UpdateSamplers();
			uint t = UpdateTextures();
			Set<&ID3D11DeviceContext::GSSetConstantBuffers,
				&ID3D11DeviceContext::GSSetSamplers,
				&ID3D11DeviceContext::GSSetShaderResources>(context, s, t);
			context->GSSetShader(mGeometryShader, null, 0);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(void const *blob, size_t size) override
		{
			DXR(Device->CreateGeometryShader(blob, size, null, &mGeometryShader));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Load(FileResource &f)
		{
			DXR(Shader::Create(FindShaderInSOBFile(f, ShaderType::Geometry)));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		DXPtr<ID3D11GeometryShader> mGeometryShader;
	};
}
