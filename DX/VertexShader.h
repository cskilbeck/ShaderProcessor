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
					 BindingState &bindState,
					 BindRun *vertexBufferBindings,
					 uint vertexBufferBindingCount)
			 : Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames, textureArray, samplerArray, bindState)
			 , mVertexBufferBindings(vertexBufferBindings)
			 , mVertexBufferBindingCount(vertexBufferBindingCount)
		{
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
			uint bp = 0;
			for(uint i = 0; i < mVertexBufferBindingCount; ++i)
			{
				BindRun &run = mVertexBufferBindings[i];

				// TODO: IN DEBUG, CHECK HERE THAT WE HAVE ENOUGH IN THE ARRAYS

				Context->IASetVertexBuffers(run.mBindPoint, run.mBindCount, handles + bp, strides + bp, offsets + bp);
				bp += run.mBindCount;
			}
		}

		//////////////////////////////////////////////////////////////////////

		void SetVertexBuffers(ID3D11DeviceContext *context, int n, TypelessBuffer **buffers)
		{
			ID3D11Buffer *handles[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
			uint strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
			uint offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
			for(int i = 0; i < n; ++i)
			{
				TypelessBuffer *b = buffers[i];
				handles[i] = b->Handle();
				strides[i] = b->Size();
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
			strides[0] = T::VertexSize();
			offsets[0] = 0;

			// any others...
			va_list v;
			va_start(v, n);
			for(int i = 1; i < n; ++i)
			{
				T *b = va_arg(v, T *);
				handles[i] = b->Handle();
				strides[i] = T::VertexSize();
				offsets[i] = 0; // For now...
			}
			va_end(v);

			// stuff them in
			SetVertexBindings(handles, strides, offsets);
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
		BindRun *							mVertexBufferBindings;
		uint32								mVertexBufferBindingCount;
	};
}