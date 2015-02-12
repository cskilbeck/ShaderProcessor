//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

namespace
{
	using namespace DX;

	enum ItemType
	{
		it_Invalid = 0,
		it_Material,
		it_Texture,
		it_Sampler,
		it_Shader,
		it_VSConstants,
		it_PSConstants,
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

	struct MaterialItem: Item
	{
		enum
		{
			eType = it_Material
		};

		Material *mMaterial;
	};

	//////////////////////////////////////////////////////////////////////

	struct TextureItem: Item
	{
		enum
		{
			eType = it_Texture
		};

		Texture *mTexture;
		uint mIndex;
	};

	//////////////////////////////////////////////////////////////////////

	struct SamplerItem: Item
	{
		enum
		{
			eType = it_Sampler
		};

		Sampler *mSampler;
		uint mIndex;
	};

	//////////////////////////////////////////////////////////////////////

	struct VSConstBufferItem: Item
	{
		enum
		{
			eType = it_VSConstants
		};

		uint32 mIndex;
		uint32 mSize;
	};

	//////////////////////////////////////////////////////////////////////

	struct PSConstBufferItem: Item
	{
		enum
		{
			eType = it_PSConstants
		};

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
		uint mVertexSize;

		void SetPSTexture(TextureItem *t)
		{
			mShader->Shaders[Pixel]->mTextures[t->mIndex] = t->mTexture;
		}

		void SetPSSampler(SamplerItem *s)
		{
			mShader->Shaders[Pixel]->mSamplers[s->mIndex] = s->mSampler;
		}

		void SetVSConstants(VSConstBufferItem *i, ID3D11DeviceContext *context)
		{
			mShader->Shaders[Vertex]->mConstBuffers[i->mIndex]->Set(context, (byte *)i + sizeof(VSConstBufferItem));
		}

		void SetPSConstants(PSConstBufferItem *i, ID3D11DeviceContext *context)
		{
			mShader->Shaders[Pixel]->mConstBuffers[i->mIndex]->Set(context, (byte *)i + sizeof(PSConstBufferItem));
		}

		void Activate(ID3D11DeviceContext *context)
		{
			mShader->Activate_V(context);
			mVertexBuffer->Commit(context);
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

		uint32 mBase;
		uint32 mCount;
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
	{
		Reset();
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::Reset()
	{
		mItemPointer = mItemBuffer;
		mVertBase = 0;
		mVertCount = 0;
		mVertPointer = null;
	}

	//////////////////////////////////////////////////////////////////////

	DrawList::~DrawList()
	{
		Delete(mItemBuffer);
		mItemPointer = null;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::SetVSConsts(byte *data, uint size, uint index)
	{
		VSConstBufferItem *i = Add<VSConstBufferItem>();
		AddData(data, size);
		i->mIndex = index;
		i->mSize = size;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::SetPSConsts(byte *data, uint size, uint index)
	{
		PSConstBufferItem *i = Add<PSConstBufferItem>();
		AddData(data, size);
		i->mIndex = index;
		i->mSize = size;
	}

	//////////////////////////////////////////////////////////////////////

	byte *DrawList::AddData(byte const *data, uint size)
	{
		memcpy(mItemPointer, data, size);
		mItemPointer += size;
		return mItemPointer;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::SetShader(ShaderState *shader, TypelessBuffer *vb, uint vertSize)
	{
		ShaderItem *i = Add<ShaderItem>();
		i->mShader = shader;
		i->mVertexBuffer = vb;
		i->mVertexSize = vertSize;
		mVertPointer = vb->Data();
		mVertCount = 0;
		mVertBase = 0;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::SetMaterial(Material &m)
	{
		Add<MaterialItem>()->mMaterial = &m;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::SetPSTexture(Texture &t, uint index)
	{
		TextureItem *ti = Add<TextureItem>();
		ti->mTexture = &t;
		ti->mIndex = index;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::SetPSSampler(Sampler &s, uint index)
	{
		SamplerItem *si = Add<SamplerItem>();
		si->mSampler = &s;
		si->mIndex = index;
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::BeginDrawCall(uint32 topology)
	{
		DrawCallItem *i = Add<DrawCallItem>();
		mCurrentDrawCallItem = (void *)i;
		i->mBase = mVertBase;
		i->mCount = 0;
		i->mTopology = topology;
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
			d->mCount = mVertCount;
			mVertBase += mVertCount;
			mVertCount = 0;
		}
	}

	//////////////////////////////////////////////////////////////////////

	void DrawList::Execute(ID3D11DeviceContext *context)
	{
		byte *end = mItemPointer;
		byte *t = mItemBuffer;

		ShaderItem *csi = null;

		while(t < end)
		{
			switch(((Item *)t)->mType)
			{
				case it_Material:
					((MaterialItem *)t)->mMaterial->Activate(context);
					t += sizeof(MaterialItem);
					break;

				case it_Texture:
					assert(csi != null);
					csi->SetPSTexture((TextureItem *)t);
					t += sizeof(TextureItem);
					break;

				case it_Sampler:
					assert(csi != null);
					csi->SetPSSampler((SamplerItem *)t);
					t += sizeof(SamplerItem);
					break;

				case it_Shader:
					csi = (ShaderItem *)t;
					t += sizeof(ShaderItem);
					break;

				case it_VSConstants:
					assert(csi != null);
					csi->SetVSConstants((VSConstBufferItem *)t, context);
					t += sizeof(VSConstBufferItem) + ((VSConstBufferItem *)t)->mSize;
					break;

				case it_PSConstants:
					assert(csi != null);
					csi->SetPSConstants((PSConstBufferItem *)t, context);
					t += sizeof(PSConstBufferItem) + ((PSConstBufferItem *)t)->mSize;
					break;

				case it_DrawCall:
					assert(csi != null);
					csi->Activate(context);
					((DrawCallItem *)t)->Execute(context);
					t += sizeof(DrawCallItem);
					break;
			}
		}

		mItemPointer = mItemBuffer;
		mVertBase = 0;
		mVertCount = 0;
	}
}
