//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
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

		Vec2f MeasureChar(wchar c, Vec2f *offsetOf = null);
		Vec2f MeasureString(char const *text, Vec2f *offset = null, vector<Link> *links = null) const;
		int GetHeight() const;
		float GetBaseline() const;
		string WrapText(string txt, uint pixelWidth, string lineBreak);

		static void SetupDrawList(ID3D11DeviceContext *context, DrawList &drawList, Window const * const window);
		static void SetupContext(ID3D11DeviceContext *context, Window const * const window);

		void Begin();
		void DrawString(char const *text, Vec2f &pos, HorizontalAlign horizAlign = HLeft, VerticalAlign vertAlign = VTop, uint layerMask = 0xffffffff);
		void End();

		tstring mName;

		list_node<Font> mListNode;

		~Font();

	private:

		Font();

		bool DrawChar(int layer, Vec2f &cursor, wchar c, Color color);

		friend struct FontManager;

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

		Map					mGlyphMap;
		Texture **			mPages;
		Glyph *				mGlyphs;
		Layer *				mLayers;
		KerningValue *		mKerningValues;
		Graphic *			mGraphics;

		static DrawList *	mDrawList;
	};

	//////////////////////////////////////////////////////////////////////

	struct FontManager
	{
		static void Init(Window *window);
		static Font *Load(tchar const *name);
	};
}