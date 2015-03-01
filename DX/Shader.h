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

	struct Shader;

	struct ShaderState: Aligned16
	{
		//////////////////////////////////////////////////////////////////////

		virtual void Activate_V(ID3D11DeviceContext *context)
		{
		}

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
			DX::Device->CreateBlendState((D3D11_BLEND_DESC const *)blendDesc, &mBlendState);
			DX::Device->CreateDepthStencilState((D3D11_DEPTH_STENCIL_DESC const *)depthStencilDesc, &mDepthStencilState);
			DX::Device->CreateRasterizerState((D3D11_RASTERIZER_DESC const *)rasterizerDesc, &mRasterizerState);
		}

		//////////////////////////////////////////////////////////////////////

		void SetState(ID3D11DeviceContext *context)
		{
			context->OMSetBlendState(mBlendState, null, 0xffffffff);
			context->OMSetDepthStencilState(mDepthStencilState, 0);
			context->RSSetState(mRasterizerState);
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct Shader
	{
		struct BindRun
		{
			uint16 mBindPoint;
			uint16 mBindCount;
		};

		template<typename T> struct BindingInfo
		{
			vector<BindRun> mBindRuns;
			vector<T *>		mPointers;
		};

		char const **						mConstBufferNames;
		char const **						mSamplerNames;
		char const **						mTextureNames;

		uint32								mNumConstBuffers;
		uint32								mNumSamplers;
		uint32								mNumTextures;

		vector<BoundBuffer *>				mConstBuffers;
		Texture **							mTextures;
		Sampler **							mSamplers;

		vector<ID3D11ShaderResourceView *>	mTexturePointers;
		vector<ID3D11SamplerState *>		mSamplerPointers;
		vector<ID3D11Buffer *>				mConstBufferPointers;

		BindingInfo<ID3D11Buffer>			mBindingInfo;

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

		Shader(uint32 numConstBuffers, char const **constBufferNames,
			   uint32 numSamplers, char const **samplerNames,
			   uint32 numTextures, char const **textureNames,
			   Texture **textureArray,
			   Sampler **samplerArray)
			   : mConstBufferNames(constBufferNames)
			   , mSamplerNames(samplerNames)
			   , mTextureNames(textureNames)
			   , mNumConstBuffers(numConstBuffers)
			   , mNumSamplers(numSamplers)
			   , mNumTextures(numTextures)
			   , mTextures(textureArray)
			   , mSamplers(samplerArray)
		{
			mTexturePointers.resize(numTextures);
			mSamplerPointers.resize(numSamplers);
		}

		uint UpdateTextures(ID3D11ShaderResourceView **ptrs)
		{
			ID3D11ShaderResourceView **p = ptrs;
			for(uint i = 0; i < mNumTextures; ++i)
			{
				if(mTextures[i] != null)
				{
					*p++ = mTextures[i]->mShaderResourceView;
				}
			}
			return p - ptrs;
		}

		uint UpdateSamplers(ID3D11SamplerState **ptrs)
		{
			ID3D11SamplerState **p = ptrs;
			for(uint i = 0; i < mNumSamplers; ++i)
			{
				if(mSamplers[i] != null)
				{
					*p++ = mSamplers[i]->mSamplerState;
				}
			}
			return p - ptrs;
		}

		void SetupConstBufferRuns()
		{
			mBindingInfo.mBindRuns.clear();
			mBindingInfo.mPointers.clear();

			for(uint i = 0; i < mConstBuffers.size(); ++i)
			{
				BindRun b;
				uint count = i;
				uint base = mConstBuffers[i]->mBindPoint;
				b.mBindPoint = (uint16)base;
				mBindingInfo.mPointers.push_back(mConstBuffers[i]->Handle());
				for(++i; i < mConstBuffers.size() && mConstBuffers[i]->mBindPoint == ++base; ++i)
				{
					mBindingInfo.mPointers.push_back(mConstBuffers[i]->Handle());
				}
				b.mBindCount = (uint16)(i - count);
				mBindingInfo.mBindRuns.push_back(b);
			}
		}

		static void GetOffsetInSOBFile(FileResource &f, ShaderType type, void const * &ptr, size_t &size)
		{
			assert(type <= NumShaderTypes);
			uint32 *off = (uint32 *)f.Data();
			size_t offset = off[type];
			ptr = (byte *)f.Data() + offset;
			size = ((type == NumShaderTypes - 1) ? f.Size() : off[type + 1]) - offset;
		}

	};
}