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

	template<char const *const constBufferNames, uint32 constBufferCount> struct ShaderBase
	{
		vector<void *> mConstBuffers;
		uint32 SamplerCount;
		uint32 ResourceCount;
		uint32 ConstBufferCount;

		char const *const *samplerNames;
		char const *const *resourceNames;
		char const *const *constBufferNames;

		template<typename T> bool GetConstBuffer(char const *name, T * &ptr)
		{
			for(uint i = 0; i < ConstBufferCount; ++i)
			{
				if(strcmp(constBufferNames[i], name) == 0)
				{
					ptr = (T *)(mConstBuffers[i]);
					return true;
				}
			}
			return false;
		}
	};

	//////////////////////////////////////////////////////////////////////

	template<typename T> struct ALIGNED(16) ConstBuffer : T
	{
		//////////////////////////////////////////////////////////////////////

		char const *					mName;
		uint32 const					mOffsetCount;
		ConstBufferOffset const * const	mOffsets;
		uint32 const * const 			mDefaults;
		ShaderBase *					mParent;
		uint32							mVertCount;
		DXPtr<ID3D11Buffer>				mConstantBuffer;

		ConstBuffer(char const *name, uint32 OffsetCount, ConstBufferOffset const Offsets[], uint32 *Defaults, ShaderBase *parent)
			: mName(name)
			, mOffsetCount(OffsetCount)
			, mOffsets(Offsets)
			, mDefaults(Defaults)
			, mParent(parent)
			, mVertCount(0)
		{
			mParent->mConstBuffers.push_back(this);
			ResetToDefaults();
			CD3D11_BUFFER_DESC desc(sizeof(T), D3D11_BIND_CONSTANT_BUFFER);
			D3D11_SUBRESOURCE_DATA srd = { 0 };
			srd.pSysMem = (void const *)this;
			DXV(gDevice->CreateBuffer(&desc, &srd, &mConstantBuffer));
		}

		//////////////////////////////////////////////////////////////////////

		void Commit(ID3D11DeviceContext *context)
		{
			context->UpdateSubresource(mConstantBuffer, 0, null, this, 0, 0);
		}

		//////////////////////////////////////////////////////////////////////

		void ResetToDefaults()
		{
			memcpy(this, mDefaults, sizeof(T)); // !!
		}

		//////////////////////////////////////////////////////////////////////

		uint32 SizeOfBuffer() const
		{
			return (uint32)(sizeof(T) * mVertCount);
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

	template<typename vert, D3D11_INPUT_ELEMENT_DESC *inputElements> struct VertexBuffer
	{
		static HRESULT Create(uint vertCount, VertexBuffer **buffer)
		{
			Ptr<VertexBuffer> t(new VertexBuffer());
			t.Resize(vertCount);
			CD3D11_BUFFER_DESC desc(sizeof(vertType), D3D11_BIND_VERTEX_BUFFER);
			DX(gDevice->CreateBuffer(&desc, &t, &(t->mD3DBuffer)));
			*buffer = t.release();
			return S_OK;
		}

		void Commit(ID3D11DeviceContext *context)
		{
			context->UpdateSubresource(mD3DBuffer, 0, null, mBuffer.data, 0, 0);
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
			return mBuffer.data;
		}

	private:

		void Resize(uint count)
		{
			mBuffer.resize(count);
		}

		vector<vert>		mBuffer;
		DXPtr<ID3D11Buffer> mD3DBuffer;
	};

	//////////////////////////////////////////////////////////////////////

} // HLSL


