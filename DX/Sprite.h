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
		static SByte4 Flips(int x, int y)
		{
			assert(x >= 0 && x <= 1 && y >= 0 && y <= 1);
			return flips[x + y * 2];
		}

		void SetFlip(int x, int y)
		{
			Flip = Flips(x, y);
		}

		void Set(
			Sprite const &o,
			Vec2f pos,
			Vec2f scale = { 1, 1 },
			Vec2f pivot = { 0.5f, 0.5f },
			float rotation = 0,
			DX::Color color = Color::White,
			bool xflip = false,
			bool yflip = false)
		{
			Position = pos;
			Pivot = pivot;
			Size = o.Size;
			Scale = scale;
			UVa = o.UVa;
			UVb = o.UVb;
			Rotation = rotation;
			Color = color;
			SetFlip(xflip, yflip);
		}
	};

	struct SpriteSheet
	{
		using Vert = Shaders::Sprite::InputVertex;

		vector<Sprite>						mSprites;
		std::map<string, int>				mSheet;
		Shaders::Sprite						mShader;
		Texture								mPage;
		Sampler								mSampler;
		VertexBuilder<Vert>					mVertexBuffer;
		Vert *								mCurrentVert;
		tstring								mName;
		uint32								mScreenWidth;
		uint32								mScreenHeight;
		uint32								mVertBase;
		DrawList *							mDrawList;

		SpriteSheet()
		{
		}

		Sprite const &operator[] (char const *name)
		{
			auto f = mSheet.find(name);
			if(f != mSheet.end())
			{
				return mSprites[f->second];
			}
			assert(false);
			static Sprite s;
			return s;
		}

		HRESULT BeginRun(ID3D11DeviceContext *context)
		{
			mVertBase = 0;	// for now
			DXR(mVertexBuffer.Map(context));
			return S_OK;
		}

		void Add(Sprite *sprite)
		{
			mVertexBuffer.AddVertex(*sprite);
		}

		void Add(Sprite const &sprite,
				 Vec2f pos,
				 Vec2f scale = { 1, 1 },
				 Vec2f pivot = { 0.5f, 0.5f },
				 float rotation = 0,
				 Color color = Color::White,
				 bool xflip = false,
				 bool yflip = false)
		{
			Sprite &s = (Sprite &)mVertexBuffer.AddVertex();
			s.Position = pos;
			s.Pivot = pivot;
			s.Size = sprite.Size;
			s.Scale = scale;
			s.UVa = sprite.UVa;
			s.UVb = sprite.UVb;
			s.Rotation = rotation;
			s.Color = color;
			s.SetFlip(xflip, yflip);
		}

		void ExecuteRun(ID3D11DeviceContext *context)
		{
			uint count = mVertexBuffer.Count() - mVertBase;
			mVertexBuffer.UnMap(context);
			mShader.Activate(context);
			uint stride = sizeof(Sprite);
			uint offset = 0;
			context->IASetVertexBuffers(0, 1, mVertexBuffer.AddressOfHandle(), &stride, &offset);
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			context->Draw(count, mVertBase);
		}

		HRESULT SetupTransform(ID3D11DeviceContext *context, int width, int height)
		{
			// TODO (charlie): MappedResource ctor can fail - breaks RTCB#n
			mShader.gs.vConstants->Get()->TransformMatrix = Transpose(OrthoProjection2D(width, height));
			return S_OK;
		}

		void SetupDrawlist(ID3D11DeviceContext *context, DrawList &d, int width, int height)
		{
			mDrawList = &d;
			mVertexBuffer.Map(context);
			mVertBase = 0;	// for now
			d.SetShader(context, &mShader, &mVertexBuffer);
			Shaders::Sprite::GS::vConstants_t vc;
			vc.TransformMatrix = Transpose(OrthoProjection2D(width, height));
			d.SetConstantData(Geometry, vc);
			d.BeginPointList();
		}

		void AddToDrawList(
				Sprite const &sprite,
				Vec2f pos,
				Vec2f scale = { 1, 1 },
				Vec2f pivot = { 0.5f, 0.5f },
				float rotation = 0,
				Color color = Color::White,
				bool xflip = false,
				bool yflip = false)
		{
			auto &q = mVertexBuffer.AddVertex();
			q.Position = pos;
			q.Pivot = pivot;
			q.Size = sprite.Size;
			q.Scale = scale;
			q.UVa = sprite.UVa;
			q.UVb = sprite.UVb;
			q.Rotation = rotation;
			q.Color = color;
			q.Flip = Sprite::Flips(xflip, yflip);
		}

		void Finished(ID3D11DeviceContext *context)
		{
			mDrawList->End();
			mVertexBuffer.UnMap(Context);
		}

		virtual ~SpriteSheet()
		{
		}

		void Release()
		{
			TRACE("SpriteSheet::Release();\n");
			mSampler.Release();
			mShader.Release();
			mPage.Release();
			mVertexBuffer.Destroy();
			mSprites.clear();
		}

		HRESULT Load(tchar const *filename)
		{
			DXR(mSampler.Create());
			DXR(mShader.Create());
			DXR(mVertexBuffer.Create(500));

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

			tstring textureName = TString(GetValue(root, L"imagePath"));

			textureName = GetPath(filename) + textureName;

			Vec2f tsize(GetFloat(root, L"width"), GetFloat(root, L"height"));

			DiskFile *page;
			DXR(AssetManager::Open(textureName.c_str(), (FileBase **)&page));
			HRESULT r = mPage.Load(page);
			page->Close();

			DXR(r);

			if(!mPage.IsValid())
			{
				return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
			}
			if(mPage.FSize() != tsize)
			{
				return HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
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
				string name = String(GetValue(spriteNode, L"n"));
				mSprites.push_back(s);
				mSheet[name] = index++;
			}
			mShader.ps.page = &mPage;
			mShader.ps.smplr = &mSampler;
			return S_OK;
		}
	};
}
