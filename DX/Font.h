//////////////////////////////////////////////////////////////////////

#pragma once

#include "Shaders\Font.Shader.h"

//////////////////////////////////////////////////////////////////////

namespace DX
{
	// rename this to TypeFace
	struct Typeface: RefCounted
	{
		struct Glyph;
		struct Layer;
		struct KerningValue;
		struct Graphic;

		struct Link
		{
			Vec2f		mTopLeft;
			Vec2f		mBottomRight;
			wstring		mText;
		};

		int GetHeight() const;
		float GetBaseline() const;

		float GetNormalizedBaseline() const
		{
			return GetBaseline() / GetHeight();
		}

		Vec2f MeasureChar(wchar c, Vec2f *offsetOf = null);
		Vec2f MeasureString(char const *text, Vec2f *offset = null, vector<Link> *links = null) const;
		string WrapText(string txt, uint pixelWidth, string lineBreak);

		tstring mName;

		list_node mListNode;

		~Typeface();

	private:

		Typeface();

		friend struct FontManager;
		friend struct Font;

		Glyph *GetGlyph(wchar c);

		HRESULT LoadFromFile(tchar const *filename);

		typedef std::unordered_map<wchar, int> Map;	// map into Glyph array per char - fix with a big reverse lookup? (would be 256K though, and mostly empty...)

		float				mBaseline;
		int					mHeight;
		int					mPageCount;
		int					mGlyphCount;
		int					mLayerCount;
		int					mKerningValueCount;
		int					mGraphicCount;
		Map					mGlyphMap;
		Texture **			mPages;
		Glyph *				mGlyphs;
		Layer *				mLayers;
		KerningValue *		mKerningValues;
		Graphic *			mGraphics;
	};

	//////////////////////////////////////////////////////////////////////

	struct Font
	{
		using VB = VertexBuilder<DXShaders::Font::InputVertex>;

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

		Typeface *		mTypeface;			// instance of which font
		DrawList *		mDrawList;			// drawlist it draws into
		int				mCurrentPageIndex;	// which page was last used to draw a char
		VB *			mVertexBuffer;		// where the verts get stashed

		Font(Typeface *typeFace= null, DrawList *drawList = null, VB *vertexBuffer = null)
			: mTypeface(typeFace)
			, mDrawList(drawList)
			, mCurrentPageIndex(-1)
			, mVertexBuffer(vertexBuffer)
		{
		}

		void Init(Typeface *typeface, DrawList *drawList, VB *vertexBuffer);
		void Start(ID3D11DeviceContext *context, Matrix const &matrix);
		void Begin(ID3D11DeviceContext *context, Matrix const &matrix);
		void Begin(ID3D11DeviceContext *context, Window const * const window);
		bool DrawChar(int layer, Vec2f &cursor, wchar c, Color color);
		void DrawString(char const *text, Vec2f &pos, HorizontalAlign horizAlign = HLeft, VerticalAlign vertAlign = VTop, uint layerMask = 0xffffffff);
		void End();
		void Finished(ID3D11DeviceContext *context);
	};

	//////////////////////////////////////////////////////////////////////

	struct FontManager
	{
		static HRESULT Open(Window *window);
		static bool IsOpen();
		static void Close();
		static HRESULT Load(tchar const *name, Typeface **fontPtr);
	};
}