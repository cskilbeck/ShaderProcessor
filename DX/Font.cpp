//////////////////////////////////////////////////////////////////////

#include "DX.h"
#include "RapidXML\rapidxml.hpp"
#include "RapidXML\xml_util.h"

//////////////////////////////////////////////////////////////////////

namespace
{
	using namespace DX;

	chs::linked_list<Typeface, &Typeface::mListNode> sAllFonts;
	bool initialised = false;
	DXShaders::Font shader;
	Sampler sampler;

	Window *fontManagerWindow;

	Delegate<WindowEvent> onDestroy = [] (WindowEvent const &)
	{
		while(!sAllFonts.empty())
		{
			Typeface *f = sAllFonts.head();
			TRACE(TEXT("Warning, dangling font %s being destructed\n"), f->mName.c_str());
			f->~Typeface();
		}
		shader.Release();
		sampler.Release();
	};
}

//////////////////////////////////////////////////////////////////////

struct Typeface::KerningValue
{
	wchar otherChar;			// if the char before me was this
	float amount;				// add this to the x cursor pos before drawing
};

//////////////////////////////////////////////////////////////////////

struct Typeface::Graphic
{
	Vec2f drawOffset;			// add this to the cursor position before drawing it
	Vec2f size;					// size in pixels
	Half2 uv[4];				// top left on the texture page
	int pageIndex;				// which page it's on
};

//////////////////////////////////////////////////////////////////////

struct Typeface::Glyph
{
	float advance;				// advance the x cursor pos this much after drawing
	wchar character;			// what character this Glyph is for

	Graphic *imageTable;
	int imageTableSize;

	KerningValue *kerningTable;
	int kerningTableSize;
};

//////////////////////////////////////////////////////////////////////

struct Typeface::Layer
{
	Color color;
	Vec2f offset;
	bool measure;
};

//////////////////////////////////////////////////////////////////////

static HRESULT LoadShader()
{
	if(!initialised)
	{
		DXR(shader.Create());
		DXR(sampler.Create());
		shader.ps.smplr = &sampler;
		initialised = true;
		TRACE("Initialized font shader!\n");
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	HRESULT FontManager::Open(Window *window)
	{
		DXR(LoadShader());
		fontManagerWindow = window;
		window->Destroyed += onDestroy;
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////

	bool FontManager::IsOpen()
	{
		return initialised == true && fontManagerWindow != null;
	}

	//////////////////////////////////////////////////////////////////////

	void FontManager::Close()
	{
		for(auto &f : sAllFonts)
		{
			TRACE("Dangling font: %s\n", f.mName.c_str());
		}
		fontManagerWindow->Destroyed -= onDestroy;
		shader.Release();
		sampler.Release();
		TRACE("FontManager::Close() completed\n");
	}

	//////////////////////////////////////////////////////////////////////

	HRESULT FontManager::Load(tchar const *name, Typeface **font)
	{
		tstring l = ToLower(name);
		for(auto &f : sAllFonts)
		{
			if(f.mName == name)
			{
				f.AddRef();
				*font = &f;
				return S_OK;
			}
		}
		*font = new Typeface();
		DXR((*font)->LoadFromFile(name));
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////

	Typeface::Typeface()
		: mPages(null)
		, mGlyphs(null)
		, mLayers(null)
		, mKerningValues(null)
		, mGraphics(null)
	{
		TRACE("Font::Font()\n");
		sAllFonts.push_back(this);
	}

	//////////////////////////////////////////////////////////////////////

	Typeface::~Typeface()
	{
		TRACE("Deleting font %s\n", mName.c_str());

		sAllFonts.remove(this);
		for(int i = 0; i<mPageCount; ++i)
		{
			Delete(mPages[i]);
		}
		Delete(mGlyphs);
		Delete(mLayers);
		Delete(mKerningValues);
		Delete(mGraphics);
	}

	//////////////////////////////////////////////////////////////////////

	HRESULT Typeface::LoadFromFile(tchar const *filename)
	{
		TRACE("Loading font %s\n", filename);
		mName = ToLower(tstring(filename));

		wstring buffer;
		xml_doc *doc = LoadUTF8XMLFile(Format(TEXT("%s.bitmapfont"), filename).c_str(), buffer);
		if(doc == null)
		{
			return E_FAIL;
		}
		node *root = doc->first_node(L"BitmapFont");

		if(root != null)
		{
			mPageCount = GetInt(root, L"PageCount");
			mHeight = GetInt(root, L"Height");
			mBaseline = GetFloat(root, L"Baseline");

			mKerningValues = null;
			mKerningValueCount = 0;

			mGraphicCount = 0;

			node *layersNode = root->first_node(L"Layers", 0, false);

			if(layersNode != null)
			{
				mLayerCount = GetInt(layersNode, L"Count");

				if(mLayerCount != 0)
				{
					mPages = new Texture *[mPageCount];
					string path = GetPath(filename) + GetFilename(filename);
					for(int i = 0; i<mPageCount; ++i)
					{
						mPages[i] = new Texture();

						// TODO (charlie): error check this
						mPages[i]->Load(Format(TEXT("%s%d.png"), path.c_str(), i).c_str());
					}

					mLayers = new Layer[mLayerCount];

					int layerIndex = 0;

					for(node *layer = layersNode->first_node(); layer != null; layer = layer->next_sibling())
					{
						Layer &la = mLayers[layerIndex++];

						la.offset = Vec2f(GetFloat(layer, L"offsetX"), GetFloat(layer, L"offsetY"));
						la.color = GetHex32(layer, L"color");
						la.measure = (layerIndex == 1);	// TODO: see editor
					}

					node *glyphsNode = root->first_node(L"Glyphs", 0, false);

					if(glyphsNode != null)
					{
						mGlyphCount = GetInt(glyphsNode, L"Count");

						if(mGlyphCount != 0)
						{
							mGlyphs = new Glyph[mGlyphCount];

							int glyphIndex = 0;

							mGraphics = new Graphic[mLayerCount * mGlyphCount];

							int graphicIndex = 0;

							for(node *glyph = glyphsNode->first_node(); glyph != null; glyph = glyph->next_sibling())
							{
								Glyph &gl = mGlyphs[glyphIndex];

								gl.character = (wchar)GetInt(glyph, L"char");
								gl.advance = GetFloat(glyph, L"advance");

								gl.imageTableSize = GetInt(glyph, L"images");
								gl.imageTable = null;

								gl.kerningTableSize = 0;
								gl.kerningTable = null;

								mGlyphMap.insert(Map::value_type(gl.character, glyphIndex));

								glyphIndex++;

								if(gl.imageTableSize != 0)
								{
									gl.imageTable = mGraphics + graphicIndex;
									node *g = glyph->first_node();

									int numGraphics = 0;

									while(g != null)
									{
										Graphic &gr = mGraphics[graphicIndex++];
										gr.pageIndex = GetInt(g, L"page");
										Texture *t = mPages[gr.pageIndex];
										float tw = t->FWidth();
										float th = t->FHeight();
										gr.drawOffset = Vec2f(GetFloat(g, L"offsetX"), GetFloat(g, L"offsetY"));
										gr.size = Vec2f(GetFloat(g, L"w"), GetFloat(g, L"h"));
										float left = GetFloat(g, L"x");
										float top = GetFloat(g, L"y");
										float right = left + gr.size.x;
										float bottom = top + gr.size.y;
										left /= tw;
										top /= th;
										right /= tw;
										bottom /= th;
										gr.uv[0] = { left, top };
										gr.uv[1] = { right, top };
										gr.uv[2] = { left, bottom };
										gr.uv[3] = { right , bottom };
										g = g->next_sibling();

										++numGraphics;

										assert(numGraphics < mLayerCount * mGlyphCount);
									}

									assert(numGraphics == gl.imageTableSize);

									mGraphicCount += numGraphics;
								}
							}
						}
					}
				}
			}
		}
		delete doc;
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////

	Vec2f Typeface::MeasureChar(wchar c, Vec2f *offsetOf)
	{
		Vec2f offset;
		float right = 0;
		float bottom = 0;
		Map::const_iterator i = mGlyphMap.find(c);	// TODO: shame to be messing with unordered_map in the font draw function!
		if(i != mGlyphMap.end())
		{
			Glyph &glyph = mGlyphs[i->second];
			right = glyph.advance;
			bottom = 0;

			int layer = mLayerCount - 1;

			if(layer < glyph.imageTableSize)
			{
				Graphic &g = glyph.imageTable[layer];

				float left = g.drawOffset.x;
				bottom = offset.y + g.drawOffset.y + g.size.y;
				float top = g.drawOffset.y;
				right = Max(right, g.drawOffset.x + g.size.x);

				if(left < offset.x)
				{
					offset.x = left;
				}

				if(top < offset.y)
				{
					offset.y = top;
				}
			}
		}
		if(offsetOf != null)
		{
			*offsetOf = offset;
		}
		return Vec2f(right, bottom);
	}

	//////////////////////////////////////////////////////////////////////

	Vec2f Typeface::MeasureString(char const *text, Vec2f *offset, vector<Link> *links) const
	{
		Vec2f dummyOffset;
		Vec2f &off = (offset == null) ? dummyOffset : *offset;
		off.Set(0, 0);
		Vec2f mmax(0.0f, 0.0f);

		float x = 0;
		float y = 0;

		int layer = mLayerCount - 1;

		bool inLink = false;
		bool oldInLink = false;
		bool inHash = false;
		bool escape = false;

		Vec2f topleft;
		Vec2f bottomRight;

		string linkString;

		char const *textBegin(null);
		char const *textEnd(null);

		bool lineStart = true;

		UTF8Decoder decoder(text);
		wchar c;
		while((c = decoder.Next()) > 0)
		{
			textEnd = decoder.currentPtr;

			if(inHash)
			{
				if(c == '#')
				{
					inHash = false;
				}
				continue;
			}
			else
			{
				if(escape)
				{
					escape = false;
				}
				else
				{
					if(c == '\\')
					{
						escape = true;
						continue;
					}
					else if(c == '@')
					{
						inLink = !inLink;
						if(inLink)
						{
							textBegin = decoder.currentPtr;
						}
						else if(links != null)
						{
							Link l;
							l.mTopLeft = topleft;
							l.mBottomRight = bottomRight;
							l.mText = wstring(textBegin + 1, textEnd);
							links->push_back(l);
						}
						continue;
					}
					else if(c == '#')
					{
						inHash = true;
						continue;
					}
					else
					{
						if(c == '\n')
						{
							x = 0;
							y += mHeight;
							lineStart = true;
						}
						else
						{
							if(lineStart)
							{
								lineStart = false;
								mmax.y += mHeight;
							}
							Map::const_iterator i = mGlyphMap.find(c);	// TODO: shame to be messing with unordered_map in the font draw function!
							if(i != mGlyphMap.end())
							{
								Glyph &glyph = mGlyphs[i->second];
								float right = x + glyph.advance;

								if(layer < glyph.imageTableSize)
								{
									Graphic &g = glyph.imageTable[layer];

									float left = g.drawOffset.x;
									//float bottom = off.y + g.drawOffset.y + g.size.y;
									float top = g.drawOffset.y;
									right = Max(right, g.drawOffset.x + g.size.x);

									if(left < off.x)
									{
										off.x = left;
									}

									if(top < off.y)
									{
										off.y = top;
									}

									//if(bottom > mmax.y)
									//{
									//	mmax.y = bottom;
									//}

									if(inLink && !oldInLink)
									{
										topleft.x = x + left;
										topleft.y = y;
									}

									bottomRight.y = y + mHeight;
								}
								if(right > mmax.x)
								{
									mmax.x = right;
								}
								bottomRight.x = right;
								x += glyph.advance;
							}
						}
					}
				}
			}
			oldInLink = inLink;
		}
		return mmax;
	}

	//////////////////////////////////////////////////////////////////////

	Typeface::Glyph *Typeface::GetGlyph(wchar c)
	{
		Map::const_iterator i = mGlyphMap.find(c);
		if(i != mGlyphMap.end())
		{
			return &mGlyphs[i->second];
		}
		return null;
	}

	//////////////////////////////////////////////////////////////////////

	int Typeface::GetHeight() const
	{
		return mHeight;
	}

	//////////////////////////////////////////////////////////////////////

	float Typeface::GetBaseline() const
	{
		return mBaseline;
	}

	//////////////////////////////////////////////////////////////////////

	string Typeface::WrapText(string txt, uint pixelWidth, string lineBreak)
	{
		int lineBreakLength = (int)lineBreak.size();
		uint lastGood = 1;
		Vec2f offset;

		while(MeasureString(txt.c_str(), &offset).x > pixelWidth)
		{
			int newGood = -1;
			for(uint i = lastGood; i < txt.size(); ++i)
			{
				if(txt[i] == ' ')
				{
					string newText = txt.substr(0, i);

					if(MeasureString(newText.c_str(), &offset).x < pixelWidth)
					{
						newGood = i;
					}
					else
					{
						break;
					}
				}
			}

			if(newGood == -1)
			{
				break;
			}
			else
			{
				lastGood = newGood;
			}

			if(lastGood >= txt.size())
			{
				break;
			}
			else
			{
				txt = txt.substr(0, lastGood) + lineBreak + txt.substr(lastGood + 1);
				lastGood += lineBreakLength;
			}
		}
		return txt;
	}

	//////////////////////////////////////////////////////////////////////

	void Font::Init(Typeface *typeface, DrawList *drawList, VB *vertexBuffer)
	{
		mTypeface = typeface;
		mDrawList = drawList;
		mCurrentPageIndex = -1;
		mVertexBuffer = vertexBuffer;
	}

	//////////////////////////////////////////////////////////////////////

	void Font::Start(ID3D11DeviceContext *context, Matrix const &matrix)
	{
		using VS = DXShaders::Font::VS;
		VS::g_VertConstants2D_t v;
		v.TransformMatrix = Transpose(matrix);
		mDrawList->SetShader(context, &shader, mVertexBuffer);
		mDrawList->SetConstantData(Vertex, v, VS::g_VertConstants2D_index);
		mCurrentPageIndex = -1;
	}

	//////////////////////////////////////////////////////////////////////

	void Font::Begin(ID3D11DeviceContext *context, Matrix const &matrix)
	{
		mVertexBuffer->Map(context);
		Start(context, matrix);
	}

	//////////////////////////////////////////////////////////////////////

	void Font::Begin(ID3D11DeviceContext *context, Window const * const window)
	{
		Begin(context, OrthoProjection2D(window->ClientWidth(), window->ClientHeight()));
	}

	//////////////////////////////////////////////////////////////////////

	void Font::Finished(ID3D11DeviceContext *context)
	{
		End();
		mVertexBuffer->UnMap(context);
	}

	//////////////////////////////////////////////////////////////////////

	void Font::End()
	{
		mDrawList->End();
	}

	//////////////////////////////////////////////////////////////////////

	bool Font::DrawChar(int layer, Vec2f &cursor, wchar c, Color color)
	{
		assert(mDrawList != null);
		bool rc = false;
		Typeface::Glyph *glyph = mTypeface->GetGlyph(c);
		if(glyph != null)
		{
			if(layer < glyph->imageTableSize)
			{
				Typeface::Graphic &graphic = glyph->imageTable[layer];
				Texture *t = mTypeface->mPages[graphic.pageIndex];
				if(mCurrentPageIndex != graphic.pageIndex)
				{
					mDrawList->End();
					mDrawList->SetTexture(Pixel, *t);
					mDrawList->BeginTriangleList();
					mCurrentPageIndex = graphic.pageIndex;
				}
				Vec2f v[4];
				v[0] = cursor + graphic.drawOffset;
				v[1] = Vec2f(v[0].x + graphic.size.x, v[0].y);
				v[2] = Vec2f(v[0].x, v[0].y + graphic.size.y );
				v[3] = v[0] + graphic.size;
				mVertexBuffer->AddVertex({ v[0], graphic.uv[0], color });
				mVertexBuffer->AddVertex({ v[1], graphic.uv[1], color });
				mVertexBuffer->AddVertex({ v[2], graphic.uv[2], color });
				mVertexBuffer->AddVertex({ v[1], graphic.uv[1], color });
				mVertexBuffer->AddVertex({ v[3], graphic.uv[3], color });
				mVertexBuffer->AddVertex({ v[2], graphic.uv[2], color });
			}
			cursor.x += glyph->advance;
			rc = true;
		}
		return rc;
	}

	//////////////////////////////////////////////////////////////////////

	void Font::DrawString(char const *text, Vec2f const &pos, Vec2f *newPos, Font::HorizontalAlign horizAlign, Font::VerticalAlign vertAlign, uint layerMask)
	{
		Vec2f drawOffset;
		Vec2f offset;
		Vec2f stringSize;
		bool measured = false;

		switch(vertAlign)
		{
			case VTop:
				offset.y = 0;
				break;

			case VBaseline:
				offset.y = -mTypeface->mBaseline;
				break;

			case VBottom:
				offset.y = -(float)mTypeface->mHeight;
				break;

			case VCentre:
				stringSize = mTypeface->MeasureString(text, &drawOffset);
				measured = true;
				offset.y = -stringSize.y / 2;
				break;
		}

		switch(horizAlign)
		{
			case HLeft:
				offset.x = 0;
				break;

			case HCentre:
				if(!measured)
				{
					stringSize = mTypeface->MeasureString(text, &drawOffset);
				}
				offset.x = -stringSize.x / 2;
				break;

			case HRight:
				if(!measured)
				{
					stringSize = mTypeface->MeasureString(text, &drawOffset);
				}
				offset.x = -stringSize.x;
				break;
		}

		Vec2f cursor;
		for(int i = 0; i < mTypeface->mLayerCount; ++i)
		{
			if((layerMask & (1 << i)) != 0)
			{
				Typeface::Layer &l = mTypeface->mLayers[i];
				cursor = pos + l.offset + offset;

				uint32 layerColor = l.color;
				uint32 currentColor = 0xffffffff;//Color::White;
				uint32 colorMask = 0xFFFFFFFF;

				bool escape = false;
				bool inHash = false;
				bool inLink = false;
				int inShift = 0;

				Vec2f linkTopLeft;
				Vec2f linkBottomRight;
				int linkChars = 0;

				Color linkColor = Color(64, 192, 224);

				UTF8Decoder decoder(text);
				wchar c;
				while((c = decoder.Next()) > 0)
				{
					if(inHash)
					{
						if(c == '#')
						{
							inHash = false;
						}
						else
						{
							inShift -= 4;

							if(inShift >= 0)
							{
								wchar cl = (wchar)tolower(c);

								if(cl == '-')
								{
									colorMask &= ~0xf << inShift;
								}
								else if(cl >= '0' && cl <= '9')
								{
									colorMask |= 0xf << inShift;
									currentColor |= (cl - '0') << inShift;
								}
								else if(cl >= 'a' && cl <= 'f')
								{
									colorMask |= 0xf << inShift;
									currentColor |= (cl - 'a' + 10) << inShift;
								}
							}
						}
						continue;
					}
					else
					{
						if(escape)
						{
							escape = false;
						}
						else
						{
							if(c == '\\')
							{
								escape = true;
								continue;
							}
							else if(c == '@')
							{
								inLink = !inLink;
								continue;
							}
							else if(c == '\n')
							{
								cursor.x = pos.x + l.offset.x + offset.x;
								cursor.y += mTypeface->mHeight;
								continue;
							}
							else if(c == '#')
							{
								if(!inHash)
								{
									inHash = true;
									colorMask = 0x00000000;
									currentColor = 0x00000000;
									inShift = 32;
								}
								continue;
							}
						}
					}

					Color col = (layerColor & ~colorMask) | (currentColor & colorMask);
					if(inLink)
					{
						++linkChars;
						col = linkColor;
					}
					DrawChar(i, cursor, c, col);
				}
			}
		}
		if(newPos != null)
		{
			*newPos = cursor;
		}
	}
}
