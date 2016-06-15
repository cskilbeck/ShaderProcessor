//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	enum ShaderType : uint32
	{
		Vertex = 0,
		Hull = 1,
		Domain = 2,
		Geometry = 3,
		Pixel = 4,
		Compute = 5,
		NumShaderTypes = 6
	};

	//////////////////////////////////////////////////////////////////////

	struct Shader: Aligned16
	{
		//////////////////////////////////////////////////////////////////////

		struct BindRun
		{
			uint16 mBindPoint;
			uint16 mBindCount;
		};

		//////////////////////////////////////////////////////////////////////

		struct BindingState
		{
			vector<BindRun> mSamplerBindings;
			vector<BindRun> mResourceBindings;
			vector<BindRun> mConstantBufferBindings;
		};

		//////////////////////////////////////////////////////////////////////

		char const **							mConstBufferNames;
		char const **							mSamplerNames;
		char const **							mTextureNames;

		uint32									mNumConstBuffers;
		uint32									mNumSamplers;
		uint32									mNumTextures;

		vector<TypelessBuffer *>				mConstBuffers;
		Texture **								mTextures;
		Sampler **								mSamplers;

		BindingState *							mBindingState;

		vector<ID3D11Buffer *>					mConstantBufferPointers;

		//////////////////////////////////////////////////////////////////////

		TypelessBuffer *FindConstBuffer(char const *name)
		{
			for(uint i = 0; i < mNumConstBuffers; ++i)
			{
				if(strcmp(mConstBufferNames[i], name) == 0)
				{
					return mConstBuffers[i];
				}
			}
			return null;
		}

		//////////////////////////////////////////////////////////////////////

		Shader(uint32 numConstBuffers, char const **constBufferNames,
			   uint32 numSamplers, char const **samplerNames,
			   uint32 numTextures, char const **textureNames,
			   Texture **textureArray,
			   Sampler **samplerArray,
			   BindingState &bindingState)
		{
			mConstBufferNames = constBufferNames;
			mSamplerNames = samplerNames;
			mTextureNames = textureNames;
			mNumConstBuffers = numConstBuffers;
			mNumSamplers = numSamplers;
			mNumTextures = numTextures;
			mTextures = textureArray;
			mSamplers = samplerArray;
			mBindingState = &bindingState;
			mConstantBufferPointers.reserve(numConstBuffers);
		}

		//////////////////////////////////////////////////////////////////////

		virtual void Release()
		{
			mConstBuffers.clear();
			// Textures and Samplers are owned by the client
			//mBindingState->mSamplerBindings.clear();
			//mBindingState->mResourceBindings.clear();
			//mBindingState->mConstantBufferBindings.clear();
		}

		//////////////////////////////////////////////////////////////////////

		virtual void SetName(tchar const *name)
		{
		}

		//////////////////////////////////////////////////////////////////////

		void *GetConstBuffer(char const *name)
		{
			for(uint i = 0; i < mNumConstBuffers; ++i)
			{
				if(strcmp(mConstBufferNames[i], name) == 0)
				{
					return mConstBuffers[i];
				}
			}
			return null;
		}

		//////////////////////////////////////////////////////////////////////

		int GetConstBufferIndex(char const *name)
		{
			for(uint i = 0; i < mNumConstBuffers; ++i)
			{
				if(strcmp(mConstBufferNames[i], name) == 0)
				{
					return i;
				}
			}
			return -1;
		}

		//////////////////////////////////////////////////////////////////////

	private:

		template <typename T> using SetAny = void(_stdcall ID3D11DeviceContext::*)(UINT, UINT, T * const *);

		using SetCB = SetAny<ID3D11Buffer>;
		using SetSS = SetAny<ID3D11SamplerState>;
		using SetSR = SetAny<ID3D11ShaderResourceView>;

		template <typename U> void UpdatePointers(U **ptrs, uint numPointers, void **pointers)
		{
			for(uint i = 0; i < numPointers; ++i)
			{
				pointers[i] = (void *)(*ptrs++)->Handle();
			}
		}

		template<typename T> void SetBindRuns(ID3D11DeviceContext *context, SetAny<T> func, vector<BindRun> const &bindings, void **pointers)
		{
			T **bp = (T **)pointers;
			for(auto b : bindings)
			{
				(context->*func)(b.mBindPoint, b.mBindCount, bp);
				bp += b.mBindCount;
			}
		}

	public:

		//////////////////////////////////////////////////////////////////////

		template <SetCB SetConstantBuffers, SetSS SetSamplers, SetSR SetShaderResources > void Set(ID3D11DeviceContext *context)
		{
			SetBindRuns<ID3D11Buffer>(context, SetConstantBuffers, mBindingState->mConstantBufferBindings, (void **)mConstantBufferPointers.data());

			void *pointers[128];

			UpdatePointers(mSamplers, mNumSamplers, pointers);
			SetBindRuns<ID3D11SamplerState>(context, SetSamplers, mBindingState->mSamplerBindings, pointers);

			UpdatePointers(mTextures, mNumTextures, pointers);
			SetBindRuns<ID3D11ShaderResourceView>(context, SetShaderResources, mBindingState->mResourceBindings, pointers);
		}

		//////////////////////////////////////////////////////////////////////

		void AddConstBuffer(TypelessBuffer *buffer, uint bindPoint)
		{
			mConstBuffers.push_back(buffer);
			mConstantBufferPointers.push_back(buffer->Handle());
		}

		//////////////////////////////////////////////////////////////////////

		static Resource FindShaderInSOBFile(Resource &f, ShaderType type)
		{
			void const *p;
			size_t s;
			GetOffsetInSOBFile(f, type, p, s);
			return MemoryResource(p, s);
		}

		//////////////////////////////////////////////////////////////////////

		static void GetOffsetInSOBFile(Resource &f, ShaderType type, void const * &ptr, size_t &size)
		{
			assert(type <= NumShaderTypes);
			uint32 *off = (uint32 *)f.Data();
			size_t offset = off[type];
			ptr = (byte *)f.Data() + offset;
			size = (size_t)(((type == NumShaderTypes - 1) ? f.Size() : off[type + 1]) - offset);
		}

		//////////////////////////////////////////////////////////////////////

		virtual HRESULT D3DCreate(void const *data, size_t size) = 0;

		//////////////////////////////////////////////////////////////////////

		virtual HRESULT Create(Resource &r)
		{
			DXR(D3DCreate(r.Data(), (size_t)r.Size()));
			return S_OK;
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct ShaderState: Aligned16
	{
		//////////////////////////////////////////////////////////////////////

		virtual void Activate_V(ID3D11DeviceContext *context) = 0;

		//////////////////////////////////////////////////////////////////////

		virtual ~ShaderState()
		{
		}

		//////////////////////////////////////////////////////////////////////

		bool IsInitialized() const
		{
			return mBlendState != null;	// janky for now
		}

		//////////////////////////////////////////////////////////////////////

		DXPtr<ID3D11BlendState>				mBlendState;
		DXPtr<ID3D11DepthStencilState>		mDepthStencilState;
		DXPtr<ID3D11RasterizerState>		mRasterizerState;

		Shader *Shaders[NumShaderTypes];

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(uint32 const *blendDesc,
					 uint32 const *depthStencilDesc,
					 uint32 const *rasterizerDesc)
		{
			DXR(DX::Device->CreateBlendState((D3D11_BLEND_DESC const *)blendDesc, &mBlendState));
			DXR(DX::Device->CreateDepthStencilState((D3D11_DEPTH_STENCIL_DESC const *)depthStencilDesc, &mDepthStencilState));
			DXR(DX::Device->CreateRasterizerState((D3D11_RASTERIZER_DESC const *)rasterizerDesc, &mRasterizerState));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		virtual void Release()
		{
			mBlendState.Release();
			mDepthStencilState.Release();
			mRasterizerState.Release();
		}

		//////////////////////////////////////////////////////////////////////

		ShaderState()
		{
		}

		//////////////////////////////////////////////////////////////////////

		void SetState(ID3D11DeviceContext *context)
		{
			context->OMSetBlendState(mBlendState, null, 0xffffffff);
			context->OMSetDepthStencilState(mDepthStencilState, 0);
			context->RSSetState(mRasterizerState);
		}
	};
}