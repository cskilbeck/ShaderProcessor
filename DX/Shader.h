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

		BindingState &							mBindingState;

		vector<ID3D11ShaderResourceView *>		mResourcePointers;
		vector<ID3D11SamplerState *>			mSamplerPointers;
		vector<ID3D11Buffer *>					mConstantBufferPointers;

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
		using SetCB = void(__stdcall ID3D11DeviceContext::*)(UINT, UINT, ID3D11Buffer * const *);
		using SetSS = void(__stdcall ID3D11DeviceContext::*)(UINT, UINT, ID3D11SamplerState * const *);
		using SetSR = void(__stdcall ID3D11DeviceContext::*)(UINT, UINT, ID3D11ShaderResourceView * const *);
	public:

		template <SetCB SetConstantBuffers, SetSS SetSamplers, SetSR SetShaderResources > void Set(ID3D11DeviceContext *context, uint s, uint t)
		{
			ID3D11Buffer **bufferPtr = mConstantBufferPointers.data();
			for(auto b : mBindingState.mConstantBufferBindings)
			{
				(context->*SetConstantBuffers)(b.mBindPoint, b.mBindCount, bufferPtr);
				bufferPtr += b.mBindCount;
			}

			ID3D11SamplerState **samplerPtr = mSamplerPointers.data();
			for(auto b : mBindingState.mSamplerBindings)
			{
				(context->*SetSamplers)(b.mBindPoint, b.mBindCount, samplerPtr);
				samplerPtr += b.mBindCount;
			}

			ID3D11ShaderResourceView **srvPtr = mResourcePointers.data();
			for(auto b : mBindingState.mResourceBindings)
			{
				(context->*SetShaderResources)(b.mBindPoint, b.mBindCount, srvPtr);
				srvPtr += b.mBindCount;
			}
		}

		//////////////////////////////////////////////////////////////////////

		Shader(uint32 numConstBuffers, char const **constBufferNames,
			   uint32 numSamplers, char const **samplerNames,
			   uint32 numTextures, char const **textureNames,
			   Texture **textureArray,
			   Sampler **samplerArray,
			   BindingState &bindingState)
			: mConstBufferNames(constBufferNames)
			, mSamplerNames(samplerNames)
			, mTextureNames(textureNames)
			, mNumConstBuffers(numConstBuffers)
			, mNumSamplers(numSamplers)
			, mNumTextures(numTextures)
			, mTextures(textureArray)
			, mSamplers(samplerArray)
			, mBindingState(bindingState)
		{
			mResourcePointers.resize(numTextures);
			mSamplerPointers.resize(numSamplers);
			mConstantBufferPointers.reserve(numConstBuffers);
		}

		//////////////////////////////////////////////////////////////////////

		uint UpdateTextures()
		{
			uint t = 0;
			for(uint i = 0; i < mNumTextures; ++i)
			{
				if(mTextures[i] != null)
				{
					mResourcePointers[t++] = mTextures[i]->mShaderResourceView;
				}
			}
			return t;
		}

		//////////////////////////////////////////////////////////////////////
		// Need to work out what we need to set
		// BindRuns are possibly not contiguous
		// Return a new bindingstate, not a UINT!

		uint UpdateSamplers()
		{
			uint s = 0;
			for(uint i = 0; i < mNumTextures; ++i)
			{
				if(mSamplers[i] != null)
				{
					mSamplerPointers[s++] = mSamplers[i]->mSamplerState;
				}
			}
			return s;
		}

		//////////////////////////////////////////////////////////////////////

		void AddConstBuffer(TypelessBuffer *buffer, uint bindPoint)
		{
			mConstBuffers.push_back(buffer);
			mConstantBufferPointers.push_back(buffer->Handle());
		}

		//////////////////////////////////////////////////////////////////////

		static Resource FindShaderInSOBFile(FileResource &f, ShaderType type)
		{
			void const *p;
			size_t s;
			GetOffsetInSOBFile(f, type, p, s);
			return MemoryResource(p, s);
		}

		//////////////////////////////////////////////////////////////////////

		static void GetOffsetInSOBFile(FileResource &f, ShaderType type, void const * &ptr, size_t &size)
		{
			assert(type <= NumShaderTypes);
			uint32 *off = (uint32 *)f.Data();
			size_t offset = off[type];
			ptr = (byte *)f.Data() + offset;
			size = ((type == NumShaderTypes - 1) ? f.Size() : off[type + 1]) - offset;
		}

		//////////////////////////////////////////////////////////////////////

		virtual HRESULT Create(void const *data, size_t size) = 0;

		//////////////////////////////////////////////////////////////////////

		virtual HRESULT Create(Resource &r)
		{
			DXR(Create(r.Data(), r.Size()));
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

		DXPtr<ID3D11BlendState>				mBlendState;
		DXPtr<ID3D11DepthStencilState>		mDepthStencilState;
		DXPtr<ID3D11RasterizerState>		mRasterizerState;

		Shader *Shaders[NumShaderTypes];

		//////////////////////////////////////////////////////////////////////

		ShaderState(uint32 const *blendDesc,
					uint32 const *depthStencilDesc,
					uint32 const *rasterizerDesc)
		{
			DXT(DX::Device->CreateBlendState((D3D11_BLEND_DESC const *)blendDesc, &mBlendState));
			DXT(DX::Device->CreateDepthStencilState((D3D11_DEPTH_STENCIL_DESC const *)depthStencilDesc, &mDepthStencilState));
			DXT(DX::Device->CreateRasterizerState((D3D11_RASTERIZER_DESC const *)rasterizerDesc, &mRasterizerState));
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