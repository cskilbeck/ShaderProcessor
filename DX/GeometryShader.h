//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct GeometryShader: Shader
	{
		GeometryShader(uint32 numConstBuffers, char const **constBufferNames,
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
			Set<&ID3D11DeviceContext::GSSetConstantBuffers,
				&ID3D11DeviceContext::GSSetSamplers,
				&ID3D11DeviceContext::GSSetShaderResources>(context);
			context->GSSetShader(mGeometryShader, null, 0);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT D3DCreate(void const *blob, size_t size) override
		{
			DXR(Device->CreateGeometryShader(blob, size, null, &mGeometryShader));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(Resource &f) override
		{
			DXR(Shader::Create(FindShaderInSOBFile(f, ShaderType::Geometry)));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		void Release() override
		{
			Shader::Release();
			mGeometryShader.Release();
		}

		//////////////////////////////////////////////////////////////////////

		void SetName(tchar const *name) override
		{
			SetDebugName(mGeometryShader, name);
		}

		//////////////////////////////////////////////////////////////////////

		DXPtr<ID3D11GeometryShader> mGeometryShader;
	};
}
