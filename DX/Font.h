//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct RefCounted
	{
		RefCounted()
			: mRefCount(1)
		{
		}

		virtual ~RefCounted()
		{
		}

		void AddRef()
		{
			++mRefCount;
		}

		void Release()
		{
			if(--mRefCount == 0)
			{
				delete this;
			}
		}

		int mRefCount;
	};

	struct Font : RefCounted
	{
		struct Link
		{
			Vec2f		mTopLeft;
			Vec2f		mBottomRight;
			wstring		mText;
		};

		enum HorizontalAlign
		{
			HLeft = 0,
			HRight = 1,
			HCentre = 2
		};

		enum VerticalAlign
		{
			VTop = 3,
			VCentre = 4,
			VBaseline = 5,
			VBottom = 6
		};

		bool DrawChar(int layer, Vec2f &cursor, ID3D11DeviceContext *context, wchar c, Color color);
		Vec2f MeasureChar(wchar c, Vec2f *offsetOf = null);
		Vec2f MeasureString(wchar const *text, Vec2f *offset = null, vector<Link> *links = null) const;
		int GetHeight() const;
		float GetBaseline() const;
		wstring WrapText(wstring txt, uint pixelWidth, wstring lineBreak);

		void DrawString(ID3D11DeviceContext *context, wstring const &text, Vec2f &pos, Font::HorizontalAlign horizAlign = HLeft, Font::VerticalAlign vertAlign = VTop);
		void DrawString(ID3D11DeviceContext *context, wchar const *text, Vec2f &pos, HorizontalAlign horizAlign = HLeft, VerticalAlign vertAlign = VTop);

		void DrawString(ID3D11DeviceContext *context, string const &text, Vec2f &pos, Font::HorizontalAlign horizAlign = HLeft, Font::VerticalAlign vertAlign = VTop);
		void DrawString(ID3D11DeviceContext *context, char const *text, Vec2f &pos, HorizontalAlign horizAlign = HLeft, VerticalAlign vertAlign = VTop);

		void BeginMultipleStrings(ID3D11DeviceContext *context);
		void DrawStringMultiple(wstring const &text, Vec2f &pos, Font::HorizontalAlign horizAlign = HLeft, Font::VerticalAlign vertAlign = VTop);
		void DrawStringMultiple(wchar const *text, Vec2f &pos, HorizontalAlign horizAlign = HLeft, VerticalAlign vertAlign = VTop);
		void DrawStringMultiple(string const &text, Vec2f &pos, Font::HorizontalAlign horizAlign = HLeft, Font::VerticalAlign vertAlign = VTop);
		void DrawStringMultiple(char const *text, Vec2f &pos, HorizontalAlign horizAlign = HLeft, VerticalAlign vertAlign = VTop);
		void EndMultipleStrings();

		tstring mName;

		list_node<Font> mListNode;

	private:

		Font();
		~Font();

		friend struct FontManager;

		void DrawStringInternal(ID3D11DeviceContext *context, wchar const *text, Vec2f &pos, Font::HorizontalAlign horizAlign, Font::VerticalAlign vertAlign);

		void LoadFromFile(tchar const *filename);

		typedef std::unordered_map<wchar, int> Map;	// map into Glyph array per char

		struct Glyph;
		struct Layer;
		struct KerningValue;
		struct Graphic;

		float				mBaseline;
		int					mHeight;
		int					mPageCount;
		int					mGlyphCount;
		int					mLayerCount;
		int					mKerningValueCount;
		int					mGraphicCount;

		int					mCurrentPageIndex;

		Map						mGlyphMap;
		Texture **				mPages;
		Glyph *					mGlyphs;
		Layer *					mLayers;
		KerningValue *			mKerningValues;
		Graphic *				mGraphics;
		ID3D11DeviceContext *	mContext;
	};

	//////////////////////////////////////////////////////////////////////

	struct FontManager
	{
		static Font *Load(tchar const *name);
		static void CleanUp();
	};
}