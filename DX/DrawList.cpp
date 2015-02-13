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
		it_GSConstants,
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
		uint32 mIndex;
	};

	//////////////////////////////////////////////////////////////////////

	struct SamplerItem: Item
	{
		enum
		{
			eType = it_Sampler
		};

		Sampler *mSampler;
		uint32 mIndex;
	};

	//////////////////////////////////////////////////////////////////////

	struct ConstBufferItem: Item
	{
		uint32 mIndex;
		uint32 mSize;
	};

	//////////////////////////////////////////////////////////////////////

	struct VSConstBufferItem: ConstBufferItem
	{
		enum
		{
			eType = it_VSConstants
		};
	};

	//////////////////////////////////////////////////////////////////////

	struct GSConstBufferItem: ConstBufferItem
	{
		enum
		{
			eType = it_GSConstants
		};
	};

	//////////////////////////////////////////////////////////////////////

	struct PSConstBufferItem: ConstBufferItem
	{
		enum
		{
			eType = it_PSConstants
		};
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

		void SetTexture(ShaderType shaderType, TextureItem *t)
		{
			mShader->Shaders[shaderType]->mTextures[t->mIndex] = t->mTexture;
		}

		void SetSampler(ShaderType shaderType, SamplerItem *s)
		{
			mShader->Shaders[shaderType]->mSamplers[s->mIndex] = s->mSampler;
		}

		void SetConstants(ShaderType shaderType, ConstBufferItem *item, ID3D11DeviceContext *context)
		{
			mShader->Shaders[shaderType]->mConstBuffers[item->mIndex]->Set(context, (byte *)item + sizeof(ConstBufferItem));
		}

		void SetPSTexture(TextureItem *t)
		{
			SetTexture(Pixel, t);
		}

		void SetPSSampler(SamplerItem *s)
		{
			SetSampler(Pixel, s);
		}

		void SetVSConstants(VSConstBufferItem *i, ID3D11DeviceContext *context)
		{
			SetConstants(Vertex, i, context);
		}

		void SetGSConstants(GSConstBufferItem *i, ID3D11DeviceContext *context)
		{
			SetConstants(Geometry, i, context);
		}

		void SetPSConstants(PSConstBufferItem *i, ID3D11DeviceContext *context)
		{
			SetConstants(Pixel, i, context);
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

	void DrawList::SetGSConsts(byte *data, uint size, uint index)
	{
		GSConstBufferItem *i = Add<GSConstBufferItem>();
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
			d->mCount = mVertCount;
			mVertBase += mVertCount;
			mVertCount = 0;
			mCurrentDrawCallItem = null;
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

				case it_GSConstants:
					assert(csi != null);
					csi->SetGSConstants((GSConstBufferItem *)t, context);
					t += sizeof(GSConstBufferItem) + ((GSConstBufferItem *)t)->mSize;
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

				default:
					assert(false && "Corrupt DrawList!");
					return;
			}
		}

		mItemPointer = mItemBuffer;
		mVertBase = 0;
		mVertCount = 0;
	}
}
