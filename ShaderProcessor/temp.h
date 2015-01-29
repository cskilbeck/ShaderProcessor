//////////////////////////////////////////////////////////////////////

namespace HLSL
{
	//////////////////////////////////////////////////////////////////////

	struct ConstBufferOffset
	{
		char const *name;
		size_t offset;
	};
	
	//////////////////////////////////////////////////////////////////////

	struct Sampler
	{

	};

	//////////////////////////////////////////////////////////////////////

	struct Texture
	{

	};

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

		template<typename T> bool GetConstBuffer(char const *name, T * &ptr)
		{
			for(uint i = 0; i < mConstBuffers.size(); ++i)
			{
				if(strcmp(constBufferNames[i], name) == 0)
				{
					ptr = (T *)(mConstBuffers[i]);
					return true;
				}
			}
			return false;
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

	//////////////////////////////////////////////////////////////////////

	template<typename definition> struct ALIGNED(16) ConstBuffer : definition	// definition MUST be POD
	{
		//////////////////////////////////////////////////////////////////////

		uint32 const					mOffsetCount;
		ConstBufferOffset const * const	mOffsets;
		uint32 const * const 			mDefaults;
		Shader *						mParent;
		DXPtr<ID3D11Buffer>				mConstantBuffer;

		ConstBuffer(uint32 OffsetCount, ConstBufferOffset const Offsets[], uint32 const *Defaults, Shader *parent)
			: mOffsetCount(OffsetCount)
			, mOffsets(Offsets)
			, mDefaults(Defaults)
			, mParent(parent)
		{
			mParent->mConstBuffers.push_back(this);
			ResetToDefaults();
			CD3D11_BUFFER_DESC desc(sizeof(definition), D3D11_BIND_CONSTANT_BUFFER);
			D3D11_SUBRESOURCE_DATA srd = { 0 };
			srd.pSysMem = (void const *)this;
			DXV(gDevice->CreateBuffer(&desc, &srd, &mConstantBuffer));
			mParent->mConstBufferPointers.push_back(mConstantBuffer);
		}

		//////////////////////////////////////////////////////////////////////

		void Commit(ID3D11DeviceContext *context)
		{
			context->UpdateSubresource(mConstantBuffer, 0, null, this, 0, 0);
		}

		//////////////////////////////////////////////////////////////////////

		void ResetToDefaults()
		{
			if(mDefaults != null)
			{
				memcpy(this, mDefaults, sizeof(definition)); // !!
			}
			else
			{
				memset(this, 0, sizeof(definition));
			}
		}

		//////////////////////////////////////////////////////////////////////

		bool GetOffset(char const *name, int &offset) const
		{
			for(int i = 0; i < mOffsetCount; ++i)
			{
				if(strcmp(mOffsets[i].name, name) == 0)
				{
					offset = mOffsets[i].offset;
					return true;
				}
			}
			return false;
		}

		//////////////////////////////////////////////////////////////////////

		void *AddressOf(char const *name) const
		{
			int offset;
			return GetOffset(name, offset) ? (byte *)this + offset : null;
		}
	};

	//////////////////////////////////////////////////////////////////////
	// make this optionally dynamic using Map/UnMap etc

	template<typename vert> struct VertexBuffer
	{
		VertexBuffer(uint vertCount)
		{
			Create(new vert[vertCount], true);
		}

		VertexBuffer(uint vertCount, vert *data)
		{
			Create(data, false);
		}

		~VertexBuffer()
		{
			if(mOwnBuffer)
			{
				Delete(mBuffer);
			}
		}

		void Commit(ID3D11DeviceContext *context)
		{
			context->UpdateSubresource(mD3DBuffer, 0, null, mBuffer.data(), 0, 0);
		}

		void Activate(ID3D11DeviceContext *context)
		{
			context->IASetVertexBuffers(0, 1, &mD3DBuffer, null, null);
		}

		vert &operator[](uint index)
		{
			return mBuffer[index];
		}

		vert *operator &()
		{
			return mBuffer.data();
		}

	private:

		void Create(vert *data, bool own)
		{
			mOwnBuffer = own;
			mBuffer = data;
			CD3D11_BUFFER_DESC desc(sizeof(vert) * vertCount, D3D11_BIND_VERTEX_BUFFER);
			DXT(gDevice->CreateBuffer(&desc, mBuffer, &(t->mD3DBuffer)));
		}

		bool				mOwnBuffer;
		vert *				mBuffer;
		DXPtr<ID3D11Buffer> mD3DBuffer;
	};

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

	//////////////////////////////////////////////////////////////////////

	struct PixelShader: Shader
	{
		PixelShader(void const *blob, size_t size, uint numConstBuffers, char const **constBufferNames, uint numSamplers, char const **samplerNames, uint numTextures, char const **textureNames)
			: Shader(numConstBuffers, constBufferNames, numSamplers, samplerNames, numTextures, textureNames)
		{
			DXV(gDevice->CreatePixelShader(blob, size, null, &mPixelShader));
		}

		void Activate(ID3D11DeviceContext *context)
		{
			context->PSSetShaderResources(0, (uint)mTexturePointers.size(), mTexturePointers.empty() ? null : mTexturePointers.data());
			context->PSSetSamplers(0, (uint)mSamplers.size(), mSamplerPointers.empty() ? null : mSamplerPointers.data());
			context->PSSetConstantBuffers(0, (uint)mConstBufferPointers.size(), mConstBufferPointers.empty() ? null : mConstBufferPointers.data());
			context->PSSetShader(mPixelShader, null, 0);
		}

		DXPtr<ID3D11PixelShader>			mPixelShader;
	};

	//////////////////////////////////////////////////////////////////////

} // HLSL


