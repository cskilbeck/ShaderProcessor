#include "DX.h"

#include "Shaders/Image2D.shader.h"
#include "Shaders/Color2D.shader.h"

//////////////////////////////////////////////////////////////////////

namespace
{
	Font::VB fontVB;

	DXShaders::Image2D image2DShader;
	DXShaders::Color2D colorShader;

	VertexBuilder<DXShaders::Image2D::InputVertex> image2DVertBuffer;
	VertexBuilder<DXShaders::Color2D::InputVertex> colorVB;

	TypelessBuffer *clipPlanesBuffer;

	linked_list<UI::Message> messages;

	Delegate<MouseEvent>		mouseMovedDelegate;
	Delegate<MouseButtonEvent>	mouseLeftButtonDownDelegate;
	Delegate<MouseButtonEvent>	mouseLeftButtonUpDelegate;
	Delegate<MouseWheelEvent>	mouseWheelDelegate;

	UI::Element *currentHover = null;
}

//////////////////////////////////////////////////////////////////////

namespace DX
{
	namespace UI
	{
		//////////////////////////////////////////////////////////////////////

		Element *Element::sCapturedElement = null;

		//////////////////////////////////////////////////////////////////////

		MouseMessage *AddMouseMessage(Message::Type type, Vec2f const &point)
		{
			MouseMessage *m = new MouseMessage();	// Hmm - need a pool allocator for these... but they're all different sizes...
			m->mType = type;						// But - they can all be freed every frame loop, so you could have a linear buffer thing
			m->mPosition = point;
			messages.push_back(m);
			return m;
		}

		//////////////////////////////////////////////////////////////////////

		MouseWheelMessage *AddMouseWheelMessage(Vec2f const &point, float delta)
		{
			MouseWheelMessage *m = new MouseWheelMessage();
			m->mType = Message::Type::MouseWheel;
			m->mPosition = point;
			m->delta = delta;
			messages.push_back(m);
			return m;
		}

		//////////////////////////////////////////////////////////////////////

		KeyboardMessage *AddKeyboardMessage(Message::Type type, uint32 key, uint32 flags)
		{
			KeyboardMessage *k = new KeyboardMessage();
			k->mType = type;
			k->mKey = key;
			k->mFlags = flags;
			messages.push_back(k);
			return k;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Open(DX::Window *w)
		{
			DXR(image2DShader.Create());
			DXR(colorShader.Create());
			DXR(image2DVertBuffer.Create(8192));
			DXR(fontVB.Create(8192));
			DXR(colorVB.Create(8192));

			clipPlanesBuffer = colorShader.vs.FindConstBuffer("g_ClipPlanes2D");

			if(clipPlanesBuffer == null)
			{
				return ERROR_NOT_FOUND;
			}

			w->MouseMoved += mouseMovedDelegate = [] (DX::MouseEvent const &m)
			{
				AddMouseMessage(Message::MouseMove, Vec2f(m.position));
			};

			w->MouseButtonPressed += mouseLeftButtonDownDelegate = [] (DX::MouseButtonEvent const &m)
			{
				AddMouseMessage(Message::MouseLeftButtonDown, Vec2f(m.position));
			};

			w->MouseButtonReleased += mouseLeftButtonUpDelegate = [] (DX::MouseButtonEvent const &m)
			{
				AddMouseMessage(Message::MouseLeftButtonUp, Vec2f(m.position));
			};

			w->MouseWheeled += mouseWheelDelegate = [] (DX::MouseWheelEvent const &m)
			{
				AddMouseWheelMessage(Vec2f(m.position), m.wheelDelta);
			};

			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		void Close()
		{
			image2DShader.Release();
			colorShader.Release();
			image2DVertBuffer.Destroy();
			fontVB.Destroy();
			colorVB.Destroy();
		}

		//////////////////////////////////////////////////////////////////////
		// You better Execute() the DrawList after this before you draw any more UI with it...

		void Draw(Element *rootElement, ID3D11DeviceContext *context, DrawList &drawList, Matrix const &ortho)
		{
			image2DVertBuffer.Map(context);
			colorVB.Map(context);
			fontVB.Map(context);
			rootElement->Draw(context, drawList, ortho);
			drawList.End();
			fontVB.UnMap(context);
			image2DVertBuffer.UnMap(context);
			colorVB.UnMap(context);
		}

		//////////////////////////////////////////////////////////////////////

		void Update(Element *rootElement, float deltaTime)
		{
			// remove any closed ones, and call onupdate()
			rootElement->Update(deltaTime);

			// setup transforms
			rootElement->UpdateTransform(IdentityMatrix);

			if(Mouse::GetMode() == Mouse::Mode::Captured)
			{
				messages.delete_all();
			}
			else
			{
				bool gotMouse = false;
				Element *oldHover = currentHover;

				// then process all the input messages
				while(!messages.empty())
				{
					Message *m = messages.pop_front();
					if(m->mType == Message::Type::MouseMove)
					{
						gotMouse = true;
					}
					rootElement->ProcessMessage(m, false);
					delete m;
				}

				// if there was no mouse move message, shove a dummy one in so animating controls get their hovering updated
				if(!gotMouse)
				{
					MouseMessage m;
					m.mType = Message::MouseDummy;
					m.mPosition = Mouse::Position;
					rootElement->ProcessMessage(&m, false);
				}

				if(currentHover != oldHover && oldHover != null)
				{
					oldHover->Clear(Element::eHovering);
					oldHover->MouseLeft.Invoke(MouseEvent(oldHover, { 0, 0 }));
				}
			}
		}

		//////////////////////////////////////////////////////////////////////

		bool Element::Overlaps(Element const &b)
		{
			if(b.mScreenMin.x > mScreenMax.x || b.mScreenMax.x < mScreenMin.x ||
			   b.mScreenMin.y > mScreenMax.y || b.mScreenMax.y < mScreenMin.y)
			{
				return false;
			}
			return RectanglesOverlap(mScreenCoordinates, b.mScreenCoordinates);
		}

		//////////////////////////////////////////////////////////////////////

		bool Element::ProcessMessage(Message *m, bool clip)
		{
			bool pick = false;
			if(IsActive())
			{
				// picking before processing children means a child cannot exceed the bounds of the parent (as far as input goes, not visually, unless parent is a cliprect)
				// is this what we want? it's how Windows does it, bit still not sure...
				pick = Pick(m);

				if(this == sCapturedElement)
				{
					pick = true;
				}
				else if(sCapturedElement != null)
				{
					pick = false;
				}

				if(!pick)
				{
					clip |= IsClipper();
				}

				pick &= !clip;

				//if(pick)
				//{
				//	debug_outline_quad2d(mScreenCoordinates, Is(eTransparent) ? Color::Black : Color::Yellow);
				//}

				// process all children
				for(auto &c : reverse(mChildren))
				{
					// if any return false (i.e. Don't Bubble please), then stop
					if(!c.ProcessMessage(m, clip))
					{
						return false;
					}
				}

				switch(m->mType)
				{
					case Message::MouseDummy:
					{
						if(pick)
						{
							currentHover = this;
							if(!Is(eHovering))
							{
								MouseEntered.Invoke(MouseEvent(this, ((MouseMessage *)m)->mPosition));
							}
							Set(eHovering);
						}
						else
						{
							Clear(eHovering);
							DeSelect();
						}
					}
					break;

					case Message::MouseMove:
					{
						if(pick)
						{
							currentHover = this;
							if(!Is(eHovering))
							{
								MouseEntered.Invoke(MouseEvent(this, ((MouseMessage *)m)->mPosition));
							}
							MouseMoved.Invoke(MouseEvent(this, ((MouseMessage *)m)->mPosition));
							Set(eHovering);
							// are they re-entering the control with the mouse still down having pressed it earlier?
						}
						else
						{
							Clear(eHovering);
							DeSelect();
						}
					}
					break;

					case Message::MouseLeftButtonDown:
					{
						if(pick)
						{
							Pressed.Invoke(PressedEvent(this, ((MouseMessage *)m)->mPosition));
							Select();
						}
						else
						{
							DeSelect();
						}
					}
					break;

					case Message::MouseLeftButtonUp:
					{
						if(pick)
						{
							Released.Invoke(ReleasedEvent(this, ((MouseMessage *)m)->mPosition));
							if(Is(eSelected))
							{
								Clicked.Invoke(ClickedEvent(this, ((MouseMessage *)m)->mPosition));
								DeSelect();
							}
						}
					}
					break;

					case Message::MouseRightButtonDown:
					{
					}
					break;

					case Message::MouseWheel:
					{
						MouseWheelMessage *mw = (MouseWheelMessage *)m;
						MouseWheeled.Invoke(MouseWheelEvent(this, mw->mPosition, mw->delta));
					}
					break;

					case Message::KeyPress:
					{
					}
					break;

					case Message::KeyRelease:
					{
					}
					break;
				}
			}
			if(!pick)
			{
				return true;
			}
			return Bubble();
		}

		//////////////////////////////////////////////////////////////////////

		void Element::Draw(ID3D11DeviceContext *context, DrawList &drawList, Matrix const &ortho, Element *clipper)
		{
			if(IsVisible())
			{
				SortChildren();

				// is this fully clipped?
				if(clipper != null && !clipper->Overlaps(*this))
				{
					//debug_solid_quad2d(mScreenCoordinates, Color::Orange & 0x80ffffff);
					return;
				}
				Drawing.Invoke(this);
				OnDraw(mTransformMatrix * ortho, context, drawList);
				for(auto &r : mChildren)
				{
					r.Draw(context, drawList, ortho, IsClipper() ? this : clipper);
				}
				OnDrawComplete(drawList);
			}
		}

		//////////////////////////////////////////////////////////////////////

		bool ClipRectangle::IsClipper() const
		{
			return true;
		}

		//////////////////////////////////////////////////////////////////////

		void ClipRectangle::OnDrawComplete(DrawList &drawList)
		{
			Vec4f p[4] = { 0 };
			drawList.SetConstantData(Vertex, clipPlanesBuffer, p);
		}

		//////////////////////////////////////////////////////////////////////

		void ClipRectangle::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			Vec2f tl { 0, 0 };
			Vec2f br = tl + mSize;
			// the corners
			Vec4f topLeft = TransformPoint(Vec4(tl.x, tl.y, 0, 0), matrix);
			Vec4f topRight = TransformPoint(Vec4(br.x, tl.y, 0, 0), matrix);
			Vec4f bottomLeft = TransformPoint(Vec4(tl.x, br.y, 0, 0), matrix);

			// the edges
			Vec4f topEdge = topRight - topLeft;
			Vec4f leftEdge = bottomLeft - topLeft;

			// the normals
			Vec4f left = Normalize(Vec4(-GetY(leftEdge), GetX(leftEdge), 0, 0));
			Vec4f top = Normalize(Vec4(GetY(topEdge), -GetX(topEdge), 0, 0));
			Vec4f right = -left;
			Vec4f bottom = -top;

			// the planes
			Vec4f planes[4] =
			{
				SetW(left, Dot(left, topLeft)),
				SetW(right, Dot(right, topRight)),
				SetW(top, Dot(top, topLeft)),
				SetW(bottom, Dot(bottom, bottomLeft))
			};

			// set clip planes into the constant buffer
			drawList.SetConstantData(Vertex, clipPlanesBuffer, planes);
		}

		//////////////////////////////////////////////////////////////////////

		void Line::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			Vec2f tl { 0, 0 };
			Vec2f br = tl + mSize;
			drawList.SetShader(context, &colorShader, &colorVB);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Color2D::VS::g_VertConstants2D_index);
			drawList.BeginLineList();
			colorVB.AddVertex({ tl, mColor });
			colorVB.AddVertex({ br, mColor });
			drawList.End();
		}

		//////////////////////////////////////////////////////////////////////

		void OutlineRectangle::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			Vec2f tl { 0, 0 };
			Vec2f br = tl + mSize;
			drawList.SetShader(context, &colorShader, &colorVB);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Color2D::VS::g_VertConstants2D_index);
			drawList.BeginLineStrip();
			colorVB.AddVertex({ tl, mColor });
			colorVB.AddVertex({ { br.x, tl.x }, mColor });
			colorVB.AddVertex({ br, mColor });
			colorVB.AddVertex({ { tl.x, br.y }, mColor });
			colorVB.AddVertex({ tl, mColor });
			drawList.End();
		}

		//////////////////////////////////////////////////////////////////////

		void FilledRectangle::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			Vec2f tl { 0, 0 };
			Vec2f br = tl + mSize;
			drawList.SetShader(context, &colorShader, &colorVB);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Color2D::VS::g_VertConstants2D_index);
			drawList.BeginTriangleStrip();
			colorVB.AddVertex({ { tl.x, br.y }, mColor });
			colorVB.AddVertex({ br, mColor });
			colorVB.AddVertex({ tl, mColor });
			colorVB.AddVertex({ { br.x, tl.x }, mColor });
			drawList.End();
		}

		//////////////////////////////////////////////////////////////////////

		void FilledShape::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			drawList.SetShader(context, &colorShader, &colorVB);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Color2D::VS::g_VertConstants2D_index);
			drawList.BeginTriangleList();
			uint16 *r = mIndices.data();
			for(uint i = 0; i < mNumTriangles; ++i)
			{
				colorVB.AddVertex({ mPoints[*r++], mColor });
				colorVB.AddVertex({ mPoints[*r++], mColor });
				colorVB.AddVertex({ mPoints[*r++], mColor });
			}
			drawList.End();
		}

		//////////////////////////////////////////////////////////////////////

		void OutlineShape::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			drawList.SetShader(context, &colorShader, &colorVB);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Color2D::VS::g_VertConstants2D_index);
			drawList.BeginLineStrip();
			Vec2f *r = mPoints.data() + mPoints.size() - 1;
			for(uint i = 0; i < mPoints.size(); ++i)
			{
				colorVB.AddVertex({ *r, mColor });
				r = mPoints.data() + i;
			}
			drawList.End();
		}

		//////////////////////////////////////////////////////////////////////

		void Label::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			if(mColor)
			{
				FilledRectangle::OnDraw(matrix, context, drawList);
			}
			Font f(mTypeface, &drawList, &fontVB);
			f.Start(context, matrix);
			f.DrawString(mText.c_str(), mOffset, null, Font::HorizontalAlign::HLeft, Font::VerticalAlign::VTop, mLayerMask);
			f.End();
		}

		//////////////////////////////////////////////////////////////////////

		void Image::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			Vec2f tl { 0, 0 };
			Vec2f br = tl + mSize;
			drawList.SetShader(context, &image2DShader, &image2DVertBuffer);
			drawList.SetTexture(Pixel, *mGraphic);
			drawList.SetSampler(Pixel, *mSampler);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Image2D::VS::g_VertConstants2D_index);
			drawList.BeginTriangleStrip();
			image2DVertBuffer.AddVertex({ { tl.x, br.y }, { 0, 1 }, 0xffffffff });
			image2DVertBuffer.AddVertex({ br, { 1, 1 }, 0xffffffff });
			image2DVertBuffer.AddVertex({ tl, { 0, 0 }, 0xffffffff });
			image2DVertBuffer.AddVertex({ { br.x, tl.y}, { 1, 0 }, 0xffffffff });
			drawList.End();
		}

		//////////////////////////////////////////////////////////////////////

		void ListRow::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			// TODO (charlie): make this less fragile and crappy
			ListBox *l = (ListBox *)(mParent->mParent);	// parent is ClipRect, parent->parent is ListBox
			SetWidth(Max(l->Width(), l->ClientWidth()));
			mColor = Is(eHovering) ? Color::White : 0;
			Label::OnDraw(matrix, context, drawList);
		}

		//////////////////////////////////////////////////////////////////////

		ScrollBar::ScrollBar(Orientation orientation)
			: FilledRectangle()
			, mDrag(false)
			, mOrientation(orientation)
		{
			//SetPickMargins(64, 64, 64, 64);
			SetColor(0xc0c0c0c0);

			MouseEntered += mMouseEnteredDelegate = [this] (MouseEvent const &e)
			{
				SetColor(Color::LightCyan);
			};

			MouseLeft += mMouseLeftDelegate = [this] (MouseEvent const &e)
			{
				SetColor(0xc0c0c0c0);
			};

			MouseMoved += mMouseMovedDelegate = [this] (MouseEvent const &e)
			{
				if(mDrag)
				{
					if((mScreenCoordinates[0].x - e.mMousePosition.x) > 64 ||
						(mScreenCoordinates[0].y - e.mMousePosition.y) > 64 ||
						(mScreenCoordinates[1].x - e.mMousePosition.x) < -64 ||
						(mScreenCoordinates[2].y - e.mMousePosition.y) < -64)
					{
						ReleaseCapture();
						mDrag = false;
					}
					else
					{
						switch(mOrientation)
						{
							case Vertical:
							{
								SetPosition({ mPosition.x, Min(Max(0.0f, e.mMousePosition.y - mDragOffset.y), mParent->mSize.y - mSize.y) });
							}
							break;

							case Horizontal:
							{
								SetPosition({ e.mMousePosition.x - mDragOffset.x, mPosition.y });
							}
							break;
						}
					}
				}
			};

			Pressed += mMouseClickedDelegate = [this] (PressedEvent const &e)
			{
				mDrag = true;
				mDragOffset = e.mMousePosition - mPosition;
				SetCapture();
			};

			Released += mMouseReleasedDelgate = [this] (ReleasedEvent const &e)
			{
				mDrag = false;
				ReleaseCapture();
			};
		}

		//////////////////////////////////////////////////////////////////////

		void ScrollBar::Move(Vec2f const &delta)
		{
			SetPosition(mPosition + delta);
		}

		//////////////////////////////////////////////////////////////////////
	}
}