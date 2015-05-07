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
		it_ScissorRect,
		it_ResetScissorRect,
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
			Shader *s = mShader->Shaders[item->mShaderType];
			TypelessBuffer *b = s->mConstBuffers[item->mIndex];
			byte *p;
			DXV(b->Map(context, p));
			memcpy(p, (byte *)item + sizeof(ConstBufferItem), item->mSize);
			b->UnMap(context);
		}

		void Activate(ID3D11DeviceContext *context)
		{
			mShader->Activate_V(context);
			uint stride = mVertexSize;
			uint offset = 0;
			ID3D11Buffer *b = mVertexBuffer->Handle();

			// have to do this here because we don't have the template info for the shader (vs)
			// so... drawing things in a drawlist is limited to one vertex stream in stream slot 0
			context->IASetVertexBuffers(0, 1, &b, &stride, &offset);
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct ScissorRectItem: Item
	{
		enum
		{
			eType = it_ScissorRect
		};

		uint32	mIndex;
		Rect2D	mRect;
	};

	//////////////////////////////////////////////////////////////////////

	struct ResetScissorRectItem: Item
	{
		enum
		{
			eType = it_ResetScissorRect
		};

		uint32	mIndex;
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
			assert(mCount < 8192);
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
		, mContext(null)
		, mVertexSize(0)
		, mVertBase(null)
		, mVertPointer(null)
		, mVertZero(null)
	{
		memset(mTextures, 0, sizeof(mTextures));
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
	// what if it's the same vertexbuffeR?

	void DrawList::SetShader(ID3D11DeviceContext *context, ShaderState *shader, TypelessBuffer *vb, uint vertSize)
	{
		if(mContext == null || shader != mCurrentShader || vb != mCurrentVertexBuffer)
		{
			UnMapCurrentVertexBuffer();

			mContext = context;
			mCurrentVertexBuffer = vb;
			mCurrentDrawCallItem = null;
			memset(mTextures, 0, sizeof(mTextures));

			ShaderItem *i = Add<ShaderItem>();
			mCurrentShader = shader;
			i->mShader = shader;
			i->mVertexBuffer = vb;
			i->mVertexSize = vertSize;
			mCurrentVertexBuffer = vb;
			DXV(vb->Map(mContext, mVertZero));
			mVertBase = mVertPointer = mVertZero;
			mVertexSize = vertSize;
		}
	}

	//////////////////////////////////////////////////////////////////////

	Texture *DrawList::GetCurrentTexture(uint index)
	{
		return mTextures[index];
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::SetTexture(ShaderType shaderType, Texture &t, uint index)
	{
		TextureItem *ti = Add<TextureItem>();
		ti->mShaderType = shaderType;
		ti->mTexture = &t;
		ti->mIndex = index;
		mTextures[index] = &t;
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

	void DrawList::SetScissorRect(Rect2D const &rect)
	{
		ScissorRectItem *si = Add<ScissorRectItem>();
		si->mRect = rect;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::ResetScissorRect()
	{
		ResetScissorRectItem *si = Add<ResetScissorRectItem>();
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

	int DrawList::End()
	{
		DrawCallItem *d = (DrawCallItem *)mCurrentDrawCallItem;
		assert(d != null);
		d->mBase = (uint32)((mVertBase - mVertZero) / mVertexSize);
		d->mCount = (uint32)((mVertPointer - mVertBase) / mVertexSize);
		assert(d->mCount >= 0 && d->mCount < 16384);
		mVertBase = mVertPointer;
		mCurrentDrawCallItem = null;
		return d->mCount;
	}

	//////////////////////////////////////////////////////////////////////

	bool DrawList::IsDrawCallInProgress() const
	{
		return mCurrentDrawCallItem != null;
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
				{
					assert(csi != null);
					csi->SetTexture((TextureItem *)t);
					t += sizeof(TextureItem);
				}
				break;

				case it_Sampler:
				{
					assert(csi != null);
					csi->SetSampler((SamplerItem *)t);
					t += sizeof(SamplerItem);
				}
				break;

				case it_Shader:
				{
					csi = (ShaderItem *)t;
					t += sizeof(ShaderItem);
				}
				break;

				case it_Constants:
				{
					assert(csi != null);
					ConstBufferItem *c = (ConstBufferItem *)t;
					csi->SetConstants(c, mContext);
					t += sizeof(ConstBufferItem) + c->mSize;
				}
				break;

				case it_DrawCall:
				{
					assert(csi != null);
					DrawCallItem *d = (DrawCallItem *)t;
					csi->Activate(mContext);
					d->Execute(mContext);
					t += sizeof(DrawCallItem);
				}
				break;

				case it_ScissorRect:
				{
					ScissorRectItem *s = (ScissorRectItem *)t;
					mContext->RSSetScissorRects(1, &s->mRect);
					t += sizeof(ScissorRectItem);
				}
				break;

				case it_ResetScissorRect:
				{
					mContext->RSSetScissorRects(0, &Rect2D(0, 0, INT_MAX, INT_MAX));
					t += sizeof(ResetScissorRectItem);
				}
				break;

				default:
				{
					assert(false && "Corrupt DrawList!");
				}
				return;
			}
		}

		mItemPointer = mItemBuffer;
		mVertBase = null;
		mVertPointer = null;
	}
}
