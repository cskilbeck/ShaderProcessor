//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

namespace
{
	using namespace DX;

	enum ItemType
	{
		it_Invalid = 0,
		it_Texture,
		it_Sampler,
		it_Shader,
		it_Constants,
		it_RenderTarget,
		it_Clear,
		it_ClearDepth,
		it_DrawCall
	};

	//////////////////////////////////////////////////////////////////////

	struct Item
	{
		enum
		{
			eType = it_Invalid
		};

		uint32 mType;
	};

	//////////////////////////////////////////////////////////////////////

	struct TextureItem: Item
	{
		enum
		{
			eType = it_Texture
		};

		ShaderType mShaderType;
		Texture *mTexture;
		uint32 mIndex;
	};

	//////////////////////////////////////////////////////////////////////

	struct SamplerItem: Item
	{
		enum
		{
			eType = it_Sampler
		};

		ShaderType mShaderType;
		Sampler *mSampler;
		uint32 mIndex;
	};

	//////////////////////////////////////////////////////////////////////

	struct ConstBufferItem: Item
	{
		enum
		{
			eType = it_Constants
		};

		ShaderType mShaderType;
		uint32 mIndex;
		uint32 mSize;
	};

	//////////////////////////////////////////////////////////////////////

	struct ShaderItem: Item
	{
		enum
		{
			eType = it_Shader
		};

		ShaderState *mShader;
		TypelessBuffer *mVertexBuffer;
		uint32 mVertexSize;

		void SetTexture(TextureItem *t)
		{
			mShader->Shaders[t->mShaderType]->mTextures[t->mIndex] = t->mTexture;
		}

		void SetSampler(SamplerItem *s)
		{
			mShader->Shaders[s->mShaderType]->mSamplers[s->mIndex] = s->mSampler;
		}

		void SetConstants(ConstBufferItem *item, ID3D11DeviceContext *context)
		{
			mShader->Shaders[item->mShaderType]->mConstBuffers[item->mIndex]->Update(context, (byte *)item + sizeof(ConstBufferItem));
		}

		void Activate(ID3D11DeviceContext *context)
		{
			mShader->Activate_V(context);
			uint stride = mVertexSize;
			uint offset = 0;
			context->IASetVertexBuffers(0, 1, &mVertexBuffer->Handle(), &stride, &offset);
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct DrawCallItem: Item
	{
		enum
		{
			eType = it_DrawCall
		};

		uint32 mCount;
		uint32 mBase;
		uint32 mTopology;

		void Execute(ID3D11DeviceContext *context)
		{
			context->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)mTopology);
			context->Draw(mCount, mBase);
		}
	};
}

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	DrawList::DrawList()
		: mItemBuffer(new byte[8192]) // TODO
		, mCurrentVertexBuffer(null)
		, mCurrentShader(null)
		, mCurrentDrawCallItem(null)
		, mItemPointer(mItemBuffer)
	{
	}

	//////////////////////////////////////////////////////////////////////

	DrawList::~DrawList()
	{
		Delete(mItemBuffer);
		mItemPointer = null;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::UnMapCurrentVertexBuffer()
	{
		if(mCurrentVertexBuffer != null && mVertBase != null)
		{
			mCurrentVertexBuffer->UnMap(mContext);
		}
		mCurrentVertexBuffer = null;
	}

	//////////////////////////////////////////////////////////////////////

	byte *DrawList::AddData(byte const *data, uint size)
	{
		memcpy(mItemPointer, data, size);
		mItemPointer += size;
		return mItemPointer;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::SetConsts(ShaderType shaderType, byte *data, uint size, uint index)
	{
		ConstBufferItem *i = Add<ConstBufferItem>();
		AddData(data, size);
		i->mShaderType = shaderType;
		i->mIndex = index;
		i->mSize = size;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::SetShader(ShaderState *shader, TypelessBuffer *vb, uint vertSize)
	{
		ShaderItem *i = Add<ShaderItem>();
		mCurrentShader = shader;
		i->mShader = shader;
		i->mVertexBuffer = vb;
		i->mVertexSize = vertSize;
		mCurrentVertexBuffer = vb;
		mVertZero = mVertBase = mVertPointer = vb->Map(mContext);
		mVertexSize = vertSize;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::SetTexture(ShaderType shaderType, Texture &t, uint index)
	{
		TextureItem *ti = Add<TextureItem>();
		ti->mShaderType = shaderType;
		ti->mTexture = &t;
		ti->mIndex = index;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::SetSampler(ShaderType shaderType, Sampler &s, uint index)
	{
		SamplerItem *si = Add<SamplerItem>();
		si->mShaderType = shaderType;
		si->mSampler = &s;
		si->mIndex = index;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::BeginDrawCall(uint32 topology)
	{
		DrawCallItem *i = Add<DrawCallItem>();
		mCurrentDrawCallItem = (void *)i;
		i->mTopology = topology;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::BeginPointList()
	{
		BeginDrawCall(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::BeginTriangleList()
	{
		BeginDrawCall(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::BeginTriangleStrip()
	{
		BeginDrawCall(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::BeginLineList()
	{
		BeginDrawCall(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::BeginLineStrip()
	{
		BeginDrawCall(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::End()
	{
		DrawCallItem *d = (DrawCallItem *)mCurrentDrawCallItem;
		if(d != null)
		{
			d->mBase = (uint32)((mVertBase - mVertZero) / mVertexSize);
			d->mCount = (uint32)((mVertPointer - mVertBase) / mVertexSize);
			mVertBase = mVertPointer;
			mCurrentDrawCallItem = null;
		}
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::Execute()
	{
		UnMapCurrentVertexBuffer();

		byte *end = mItemPointer;
		byte *t = mItemBuffer;

		ShaderItem *csi = null;

		while(t < end)
		{
			switch(((Item *)t)->mType)
			{
				case it_Texture:
					assert(csi != null);
					csi->SetTexture((TextureItem *)t);
					t += sizeof(TextureItem);
					break;

				case it_Sampler:
					assert(csi != null);
					csi->SetSampler((SamplerItem *)t);
					t += sizeof(SamplerItem);
					break;

				case it_Shader:
					csi = (ShaderItem *)t;
					t += sizeof(ShaderItem);
					break;

				case it_Constants:
					assert(csi != null);
					csi->SetConstants((ConstBufferItem *)t, mContext);
					t += sizeof(ConstBufferItem) + ((ConstBufferItem *)t)->mSize;
					break;

				case it_DrawCall:
					assert(csi != null);
					csi->Activate(mContext);
					((DrawCallItem *)t)->Execute(mContext);
					t += sizeof(DrawCallItem);
					break;

				default:
					assert(false && "Corrupt DrawList!");
					return;
			}
		}

		mItemPointer = mItemBuffer;
		mVertBase = null;
		mVertPointer = null;
	}
}
