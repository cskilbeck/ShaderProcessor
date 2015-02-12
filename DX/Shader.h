//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	enum ShaderType
	{
		Vertex = 0,
		Hull = 1,
		Domain = 2,
		Geometry = 3,
		Pixel = 4,
		Compute = 5,
		NumShaderTypes = 6
	};

	struct Shader
	{
		char const **						mConstBufferNames;
		char const **						mSamplerNames;
		char const **						mTextureNames;

		uint32								mNumConstBuffers;
		uint32								mNumSamplers;
		uint32								mNumTextures;

		vector<TypelessBuffer *>			mConstBuffers;
		Texture **							mTextures;
		Sampler **							mSamplers;

		vector<ID3D11Buffer *>				mConstBufferPointers;
		vector<ID3D11ShaderResourceView *>	mTexturePointers;
		vector<ID3D11SamplerState *>		mSamplerPointers;

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
			mConstBufferPointers.reserve(numConstBuffers);
		}

		void UpdateConstants(uint index, void *data, uint32 size)
		{
			TypelessBuffer *b = (TypelessBuffer *)(mConstBuffers[index]);
		}

		void UpdateTextures()
		{
			for(uint i = 0; i < mNumTextures; ++i)
			{
				mTexturePointers[i] = (mTextures[i] != null) ? mTextures[i]->mShaderResourceView : null;
			}
		}

		void UpdateSamplers()
		{
			for(uint i = 0; i < mNumSamplers; ++i)
			{
				mSamplerPointers[i] = (mSamplers[i] != null) ? mSamplers[i]->mSamplerState : null;
			}
		}

	};

	struct ShaderState : Aligned16
	{
		virtual void Activate_V(ID3D11DeviceContext *context) = 0;

		Shader *Shaders[NumShaderTypes];
	};
}