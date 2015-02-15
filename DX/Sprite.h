//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

#include "RapidXML\rapidxml.hpp"
#include "RapidXML\xml_util.h"
#include "Shaders\Sprite.shader.h"

namespace
{
	using namespace DX;

	template<int xf, int yf> inline SByte4 Flip()
	{
		static_assert(xf >= 0 && xf <= 1 && yf >= 0 && yf <= 1, "Flip: 0 or 1 for x & y");
		int x = xf * 255;
		int y = yf * 255;
		return { (SByte)(x & 127), (SByte)(y & 127), (SByte)(127 - x), (SByte)(127 - y) };
	}

	extern WEAKSYM SByte4 flips[4] =
	{
		{ Flip<0, 0>() },
		{ Flip<1, 0>() },
		{ Flip<0, 1>() },
		{ Flip<1, 1>() }
	};
}

namespace DX
{
	struct Sprite: Shaders::Sprite::InputVertex
	{
		void SetFlip(int x, int y)
		{
			assert(x >= 0 && x <= 1 && y >= 0 && y <= 1);
			Flip = flips[x + y * 2];
		}
	};

	struct SpriteSheet
	{
		using Vert = Shaders::Sprite::InputVertex;

		vector<Sprite>						mSprites;
		std::map<string, int>				mSheet;
		Ptr<Shaders::Sprite>				mShader;
		Ptr<Texture>						mPage;
		Ptr<Sampler>						mSampler;
		Ptr<VertexBuffer<Vert>>				mVertexBuffer;
		Vert *								mCurrentVert;
		Material							mMaterial;
		tstring								mName;
		uint32								mScreenWidth;
		uint32								mScreenHeight;
		Shaders::Sprite::InputVertex *		mVertBase;
		Shaders::Sprite::InputVertex *		mVertPointer;

		SpriteSheet()
		{
		}

		Sprite *operator[] (char const *name)
		{
			auto f = mSheet.find(name);
			if(f != mSheet.end())
			{
				return &mSprites[f->second];
			}
			return null;
		}

		void BeginRun(ID3D11DeviceContext *context)
		{
			mVertBase = mVertexBuffer->Map(context);
			mVertPointer = mVertBase;
		}

		void Add(Sprite *sprite)
		{
			*mVertPointer++ = *sprite;
		}

		void Add(Sprite *sprite,
				 Vec2f pos,
				 Vec2f scale = { 1, 1 },
				 Vec2f pivot = { 0.5f, 0.5f },
				 float rotation = 0,
				 Color color = Color::White,
				 bool xflip = false,
				 bool yflip = false)
		{
			Sprite *s = (Sprite *)mVertPointer++;
			s->Position = pos;
			s->Pivot = pivot;
			s->Size = sprite->Size;
			s->Scale = scale;
			s->UVa = sprite->UVa;
			s->UVb = sprite->UVb;
			s->Rotation = rotation;
			s->Color = color;
			s->SetFlip(xflip, yflip);
		}

		void ExecuteRun(ID3D11DeviceContext *context)
		{
			uint count = mVertPointer - mVertBase;
			mShader->Activate(context);
			mMaterial.Activate(context);
			mVertexBuffer->UnMap(context);
			mVertexBuffer->Activate(context);
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			context->Draw(count, 0);
		}

		void SetScreenSize(ID3D11DeviceContext *context, int width, int height)
		{
			mShader->gs.vConstants.TransformMatrix = Transpose(Camera::OrthoProjection2D(width, height));
			mShader->gs.vConstants.Commit(context);
		}

		SpriteSheet(ID3D11DeviceContext *context, tchar const *filename)
		{
			DXT(Load(filename));
			mSampler.reset(new Sampler());
			mShader.reset(new Shaders::Sprite());
			mShader->ps.page = mPage.get();
			mShader->ps.smplr = mSampler.get();
			MaterialOptions o;
			o.blend = BlendEnabled;
			o.depth = DepthDisabled;
			o.depthWrite = DepthWriteDisabled;
			o.culling = CullingNone;
			DXT(mMaterial.Create(o));
			mVertexBuffer.reset(new Shaders::Sprite::VertBuffer(500, null, DynamicUsage, Writeable));
		}

		~SpriteSheet()
		{
		}

		HRESULT Load(tchar const *filename)
		{
			mSheet.clear();
			mSprites.clear();
			mPage.reset();
			mSampler.reset();

			wstring buffer;
			xml_doc *doc = LoadUTF8XMLFile(filename, buffer);
			if(doc == null)
			{
				return ERROR_FILE_NOT_FOUND;
			}
			node *root = doc->first_node(L"TextureAtlas");

			if(root == null)
			{
				return ERROR_BAD_FORMAT;
			}

			mName = GetFilename(filename);

			tstring textureName = TStringFromWideString(GetValue(root, L"imagePath"));

			textureName = GetPath(filename) + textureName;

			Vec2f tsize(GetFloat(root, L"width"), GetFloat(root, L"height"));

			mPage.reset(new Texture(textureName.c_str()));
			if(!mPage->IsValid())
			{
				return ERROR_FILE_NOT_FOUND;
			}
			if(mPage->FSize() != tsize)
			{
				return ERROR_BAD_FORMAT;
			}

			int index = 0;
			for(node *spriteNode = root->first_node(L"sprite", 0, false); spriteNode != null; spriteNode = spriteNode->next_sibling())
			{
				Sprite s;
				Vec2f pos = GetVec2f(spriteNode, L"x", L"y");
				Vec2f size = GetVec2f(spriteNode, L"w", L"h");
				Vec2f pivot = GetVec2f(spriteNode, L"pX", L"pY");
				s.Position = Vec2f(0, 0);
				s.Color = 0xffffffff;
				s.Flip = flips[0];
				s.Pivot = pivot;
				s.Rotation = 0;
				s.Scale = Vec2f(1, 1);
				s.Size = size;
				s.UVa = pos / tsize;
				s.UVb = (pos + size) / tsize;
				string name = StringFromWideString(GetValue(spriteNode, L"n"));
				mSprites.push_back(s);
				mSheet[name] = index++;
			}
			return S_OK;
		}
	};
}
