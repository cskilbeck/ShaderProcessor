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

	void DrawList::SetShader(ID3D11DeviceContext *context, ShaderState *shader, TypelessBuffer *vb, uint vertSize)
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
					csi->SetTexture((TextureItem *)t);
					t += sizeof(TextureItem);
					break;

				case it_Sampler:
					csi->SetSampler((SamplerItem *)t);
					t += sizeof(SamplerItem);
					break;

				case it_Shader:
					csi = (ShaderItem *)t;
					t += sizeof(ShaderItem);
					break;

				case it_Constants:
					csi->SetConstants((ConstBufferItem *)t, mContext);
					t += sizeof(ConstBufferItem) + ((ConstBufferItem *)t)->mSize;
					break;

				case it_DrawCall:
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
