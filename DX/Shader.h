//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct Shader
{
	char const **						mConstBufferNames;
	char const **						mSamplerNames;
	char const **						mTextureNames;

	uint32								mNumConstBuffers;
	uint32								mNumSamplers;
	uint32								mNumTextures;

	vector<void *>						mConstBuffers;
	vector<Texture *>					mTexture2Ds;
	vector<Sampler *>					mSamplers;

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

	Shader(uint32 numConstBuffers, char const **constBufferNames,
		   uint32 numSamplers, char const **samplerNames,
		   uint32 numTextures, char const **textureNames)
		   : mConstBufferNames(constBufferNames)
		   , mSamplerNames(samplerNames)
		   , mTextureNames(textureNames)
		   , mNumConstBuffers(numConstBuffers)
		   , mNumSamplers(numSamplers)
		   , mNumTextures(numTextures)
	{
	}
};

