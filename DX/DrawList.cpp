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
	// Base action item

	struct Item
	{
		enum
		{
			eType = it_Invalid
		};

		uint32 mType;
	};

	//////////////////////////////////////////////////////////////////////
	// Set a texture slot

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
	// Set a sampler slot

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
	// Copy some data into a constbuffer

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
	// Set a new active ShaderState

	struct ShaderItem: Item
	{
		enum
		{
			eType = it_Shader
		};

		ShaderState *mShader;
		uint32 mVertexSize;
		VertexBuilderBase *mVertexBuilder;

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
			ID3D11Buffer *b = mVertexBuilder->Handle();

			// have to do this here because we don't have the template info for the shader (vs)
			// so... drawing things in a drawlist is limited to one vertex stream in stream slot 0
			context->IASetVertexBuffers(0, 1, &b, &stride, &offset);
		}
	};

	//////////////////////////////////////////////////////////////////////
	// Set a Scissor rect

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
	// remove Scissor rect

	struct ResetScissorRectItem: Item
	{
		enum
		{
			eType = it_ResetScissorRect
		};

		uint32	mIndex;
	};

	//////////////////////////////////////////////////////////////////////
	// Actually draw something
	// TODO (charlie): check mCount is valid wrt current vertexbuffer

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
		, mContext(null)
	{
	}

	//////////////////////////////////////////////////////////////////////

	DrawList::~DrawList()
	{
		Delete(mItemBuffer);
		mItemPointer = null;
	}

	//////////////////////////////////////////////////////////////////////

	byte *DrawList::AddData(byte const *data, uint size)
	{
		memcpy(mItemPointer, data, size);
		mItemPointer += size;
		return mItemPointer;
	}
	
	//////////////////////////////////////////////////////////////////////

	void DrawList::SetShader(ID3D11DeviceContext *context, ShaderState *shader, VertexBuilderBase *vb)
	{
		assert(mContext == null || context == mContext);

		mContext = context;
		if(shader != mCurrentShader)	// this is all mCurrentShader is used for - is there a better way?
		{
			ShaderItem *i = Add<ShaderItem>();
			mCurrentShader = i->mShader = shader;
			mCurrentVertexBuffer = i->mVertexBuilder = vb;
		}
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
		End();
		DrawCallItem *i = Add<DrawCallItem>();
		VertexBuilderBase *vb = mCurrentVertexBuffer;
		mCurrentDrawCallItem = i;
		i->mTopology = topology;
		i->mBase = vb->Count();
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
			VertexBuilderBase *vb = mCurrentVertexBuffer;
			d->mCount = vb->Count() - d->mBase;
			assert(d->mCount >= 0 && d->mCount < 16384);
			mCurrentDrawCallItem = null;
		}
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::Execute()
	{
		End();

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
		mCurrentShader = null;
	}
}
