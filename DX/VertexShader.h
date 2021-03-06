//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct VertexShader: Shader
	{
		//////////////////////////////////////////////////////////////////////

		VertexShader(uint32 numConstBuffers, char const **constBufferNames,
						uint32 numSamplers, char const **samplerNames,
						uint32 numTextures, char const **textureNames,
						Texture **textureArray,
						Sampler **samplerArray,
						BindingState &bindingState,
						BindRun *vertexBufferBindings,
						uint vertexBufferBindingCount)
				: Shader(numConstBuffers, constBufferNames,
					numSamplers, samplerNames,
					numTextures, textureNames,
					textureArray,
					samplerArray,
					bindingState)
		{
			mVertexBufferBindings = vertexBufferBindings;
			mVertexBufferBindingCount = vertexBufferBindingCount;
		}

		//////////////////////////////////////////////////////////////////////

		void Release() override
		{
			Shader::Release();
			mVertexShader.Release();
			mInputLayout.Release();
		}

		//////////////////////////////////////////////////////////////////////

		void Activate(ID3D11DeviceContext *context)
		{
			Set<&ID3D11DeviceContext::VSSetConstantBuffers,
				&ID3D11DeviceContext::VSSetSamplers,
				&ID3D11DeviceContext::VSSetShaderResources>(context);
			context->VSSetShader(mVertexShader, null, 0);
			context->IASetInputLayout(mInputLayout);
		}

		//////////////////////////////////////////////////////////////////////

		void SetVertexBindings(ID3D11Buffer **handles, uint *strides, uint *offsets)
		{
			for(uint i = 0; i < mVertexBufferBindingCount; ++i)
			{
				BindRun &run = mVertexBufferBindings[i];

				// TODO: IN DEBUG, CHECK HERE THAT WE HAVE ENOUGH IN THE ARRAYS

				Context->IASetVertexBuffers(run.mBindPoint, run.mBindCount, handles, strides, offsets);
				handles += run.mBindCount;
				strides += run.mBindCount;
				offsets += run.mBindCount;
			}
		}

		//////////////////////////////////////////////////////////////////////

		template<typename T> void SetVertexBuffers(ID3D11DeviceContext *context, int n, T **buffers)
		{
			ID3D11Buffer *handles[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
			uint strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
			uint offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
			for(int i = 0; i < n; ++i)
			{
				T *b = buffers[i];
				handles[i] = b->Handle();
				strides[i] = b->DataSize();
				offsets[i] = 0; // For now
			}
			SetVertexBindings(handles, strides, offsets);
		}

		//////////////////////////////////////////////////////////////////////

		template<typename T> void SetVertexBuffers(ID3D11DeviceContext *context, int n, T *first, ...)
		{
			// get all the pointers into an array
			ID3D11Buffer *handles[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
			uint strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
			uint offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

			// first one guaranteed
			handles[0] = first->Handle();
			strides[0] = first->DataSize();
			offsets[0] = 0;

			// any others...
			if(n > 1)
			{
				va_list v;
				va_start(v, first);
				for(int i = 1; i < n; ++i)
				{
					T *b = va_arg(v, T *);
					handles[i] = b->Handle();
					strides[i] = b->DataSize();
					offsets[i] = 0; // For now...
				}
				va_end(v);
			}

			// stuff them in
			SetVertexBindings(handles, strides, offsets);
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT D3DCreate(void const *blob, size_t size) override
		{
			DXR(Device->CreateVertexShader(blob, size, null, &mVertexShader));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(Resource &f, D3D11_INPUT_ELEMENT_DESC const *inputElements, uint inputElementCount)
		{
			Resource r = FindShaderInSOBFile(f, ShaderType::Vertex);
			DXR(Shader::Create(r));
			DXR(Device->CreateInputLayout(inputElements, inputElementCount, r.Data(), (size_t)r.Size(), &mInputLayout));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		void SetName(tchar const *name) override
		{
			SetDebugName(mVertexShader, name);
			SetDebugName(mInputLayout, name);
		}

		//////////////////////////////////////////////////////////////////////

		DXPtr<ID3D11VertexShader>			mVertexShader;
		DXPtr<ID3D11InputLayout>			mInputLayout;
		BindRun *							mVertexBufferBindings;
		uint32								mVertexBufferBindingCount;
	};
}