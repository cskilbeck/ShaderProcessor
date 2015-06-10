//////////////////////////////////////////////////////////////////////
// TODO (charlie): fix how janky and lame this is
// TODO (charlie): scrollbars
// TODO (charlie): cliprectangles
// TODO (charlie): horizontal/verticalline
// TODO (charlie): listview
// TODO (charlie): input
// TODO (charlie): checkbox
// TODO (charlie): roundedRectangle
// TODO (charlie): slider
// TODO (charlie): icons (X, _, etc) ?
// TODO (charlie): etc
// TODO (charlie): changing transform might change whether the stationary mouse is over the control
// TODO (charlie): rounded rectangle? Can't do a roundedClipRect in just 8 clip planes... perhaps a stencil based clip mask thingy?

// Window: ClipRectangle, [ScrollBars], {ClientSize, ClientPos}

// TODO (charlie): event suppression/bubbling

// NOTE: cliprects do not 'stack' or combine. Each cliprect sets the 4 clip planes independently of any parent clip plane...

// Need a basic sort of RTTI /OR/ a generic message passing system

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	namespace UI
	{
		struct Element;
		struct Message;
		struct MouseMessage;
		struct KeyboardMessage;
		struct Message;

		//////////////////////////////////////////////////////////////////////
		// A Mouse or Keyboard (or whatever) message

		struct Message: list_node
		{
			enum Type
			{
				MouseMove,
				MouseWheel,
				MouseLeftButtonDown,
				MouseLeftButtonUp,
				MouseRightButtonDown,
				MouseRightButtonUp = 4,
				KeyPress,
				KeyRelease,
				MouseDummy,
				Messager
			};

			Type		mType;

			virtual bool Pick(Element const *e) const = 0;
		};

		// All mouse messages derive from this (or are this)

		struct MouseMessage: Message
		{
			Vec2f		mPosition;

			bool Pick(Element const *e) const override;
		};

		struct MouseWheelMessage: MouseMessage
		{
			float delta;
		};

		// All keyboard messages derive from this (or are this)

		struct KeyboardMessage: Message
		{
			uint32		mKey;
			uint32		mFlags;

			bool Pick(Element const *e) const override
			{
				return false;
			}
		};

		//////////////////////////////////////////////////////////////////////

		HRESULT Open(Window *w);
		MouseMessage *AddMouseMessage(Message::Type type, Vec2f const &point);
		MouseWheelMessage *AddMouseWheelMessage(Vec2f const &point, int delta);
		KeyboardMessage *AddKeyboardMessage(Message::Type type, uint32 key, uint32 flags);
		void Update(Element *rootElement, float deltaTime);
		void Draw(Element *rootElement, ID3D11DeviceContext *context, DrawList &drawList, Matrix const &ortho);
		void Close();

		//////////////////////////////////////////////////////////////////////
		// These are for notifying Elements that something has happened

		struct UIEvent
		{
			Element *mElement;

			UIEvent(Element *element)
				: mElement(element)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Mouse moved

		struct MouseEvent: UIEvent
		{
			Vec2f mMousePosition;

			MouseEvent(Element *element, Vec2f const &mousePosition)
				: UIEvent(element)
				, mMousePosition(mousePosition)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct MouseWheelEvent: MouseEvent
		{
			float wheelDelta;

			MouseWheelEvent(Element *element, Vec2f const &pos, float delta)
				: MouseEvent(element, pos)
				, wheelDelta(delta)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Mouse clicked

		struct ClickedEvent: MouseEvent
		{
			ClickedEvent(Element *element, Vec2f const &mousePosition)
				: MouseEvent(element, mousePosition)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Mouse down

		struct PressedEvent: MouseEvent
		{
			PressedEvent(Element *element, Vec2f const &mousePosition)
				: MouseEvent(element, mousePosition)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Mouse up

		struct ReleasedEvent: MouseEvent
		{
			ReleasedEvent(Element *element, Vec2f const &mousePosition)
				: MouseEvent(element, mousePosition)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////

		enum
		{
			eScrolled = 1
		};

		struct MessengerEvent: UIEvent
		{
			void *data;
			uint32 msg;

			MessengerEvent(Element *element, uint message, void *messageData)
				: UIEvent(element)
				, data(messageData)
				, msg(message)
			{
			}
		};

		// TODO (charlie): add all the other UI Event types

		//////////////////////////////////////////////////////////////////////

		struct Element: Aligned16, list_node
		{
			Matrix					mMatrix;					// local matrix
			Matrix					mTransformMatrix;			// matrix including parent transform
			Matrix					mInverseMatrix;				// inverse of mTransformMatrix for picking
			Matrix					mClientTransform;			// add this transform to all children (for scrolling etc)
			Element *				mParent;					// Parent of this Element - only the root can have no parent (I think)
			linked_list<Element>	mChildren;					// Children of this Element
			int32					mZIndex;					// for sorting
			Flags32					mFlags;						// See enum below
			Vec2f					mPosition;					// Position in local coordinates
			Vec2f					mSize;						// Size in pixels
			Vec2f					mScale;						// Scale (1,1 = not scaled)
			Vec2f					mPivot;						// Pivot point for scale/rotate (0.5,0.5f = centre)
			float					mAngle;						// Rotation angle
			Vec2f					mScreenCoordinates[4];
			Vec2f					mScreenMin;
			Vec2f					mScreenMax;

			// Events

			Event<ClickedEvent>		Clicked;
			Event<PressedEvent>		Pressed;
			Event<ReleasedEvent>	Released;
			Event<UIEvent>			Updating;
			Event<UIEvent>			Resized;
			Event<MouseWheelEvent>	MouseWheeled;
			Event<MouseEvent>		MouseMoved;
			Event<MouseEvent>		MouseEntered;
			Event<MouseEvent>		MouseLeft;
			Event<MouseEvent>		Hovering;
			Event<MessengerEvent>	Messenger;

			static Element *		sCapturedElement;

			// Flags

			enum: uint32
			{
				eHidden = 1,			// draw it or don't
				eInActive = 2,			// call OnUpdate() or don't
				eEnabled = 4,			// process input messages or don't
				eClosed = 8,			// close requested, will happen at the end of the frame
				eModal = 16,			// block parental input
				eReorderRequired = 32,	// child was added, sort children
				eDirtyMatrix = 64,		// Matrix needs to be rebuilt
				eHovering = 128,		// Mouse is currently hovering over the Element
				ePressed = 256,			// Mouse button is pressed down on the Element
				eTransparent = 512		// Mouse events pass through it
			};
			
			virtual bool Bubble() const
			{
				return Is(eTransparent);
			}

			Element()
				: mParent(null)
				, mZIndex(0)
				, mFlags(0)
				, mPosition(0, 0)
				, mSize(0, 0)
				, mScale(1, 1)
				, mPivot(0, 0)
				, mAngle(0)
				, mMatrix(IdentityMatrix)
				, mTransformMatrix(IdentityMatrix)
				, mInverseMatrix(IdentityMatrix)
				, mClientTransform(IdentityMatrix)
			{
			}

			//////////////////////////////////////////////////////////////////////

			bool IsAnyOf(uint32 f) const
			{
				return mFlags.IsAnySet(f);
			}

			//////////////////////////////////////////////////////////////////////

			bool Is(uint32 f) const
			{
				return mFlags.AreAllSet(f);
			}

			//////////////////////////////////////////////////////////////////////

			Element &Set(uint32 f)
			{
				mFlags.Set(f);
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			Element &Clear(uint32 f)
			{
				mFlags.Clear(f);
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			Element &SetFlag(uint32 f, bool v = true)
			{
				return v ? Set(f) : Clear(f);
			}

			//////////////////////////////////////////////////////////////////////

			bool IsActive() const
			{
				return !Is(eInActive);
			}

			//////////////////////////////////////////////////////////////////////

			bool IsVisible() const
			{
				return !Is(eHidden);
			}

			//////////////////////////////////////////////////////////////////////

			Element &SetVisible(bool s)
			{
				return SetFlag(eHidden, !s);
			}

			//////////////////////////////////////////////////////////////////////

			Element &Show()
			{
				return SetVisible(true);				
			}

			//////////////////////////////////////////////////////////////////////

			Element &Hide()
			{
				return SetVisible(false);
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f GetPosition() const
			{
				return mPosition;
			}

			//////////////////////////////////////////////////////////////////////

			Element &SetPosition(Vec2f pos)
			{
				mPosition = pos;
				Set(eDirtyMatrix);
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f GetSize() const
			{
				return mSize;
			}

			//////////////////////////////////////////////////////////////////////

			float Width() const
			{
				return mSize.x;
			}

			//////////////////////////////////////////////////////////////////////

			float Height() const
			{
				return mSize.y;
			}

			//////////////////////////////////////////////////////////////////////

			virtual Element &SetSize(Vec2f size)
			{
				mSize = size;
				Set(eDirtyMatrix);
				Resized.Invoke(UIEvent(this));
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f GetPivot() const
			{
				return mPivot;
			}

			//////////////////////////////////////////////////////////////////////

			Element &SetPivot(Vec2f pivot)
			{
				mPivot = pivot;
				Set(eDirtyMatrix);
				return *this;
			}

			//////////////////////////////////////////////////////////////////////
			// All mouse messages go to this control until ReleaseCapture() is called
			// or some other control calls SetCapture()

			void SetCapture()
			{
				sCapturedElement = this;
			}

			//////////////////////////////////////////////////////////////////////

			void ReleaseCapture()
			{
				sCapturedElement = null;
			}

			//////////////////////////////////////////////////////////////////////

			Matrix const &GetMatrix()
			{
				if(Is(eDirtyMatrix))
				{
					Vec2f p = mPivot * -mSize;
					mMatrix =
						TranslationMatrix(Vec4(p.x, p.y, 0)) *					// place it around the pivot point
						ScaleMatrix(Vec4(mScale.x, mScale.y, 1)) *				// scale it
						RotationMatrix(0, 0, mAngle) *							// rotate it
						TranslationMatrix(Vec4(mPosition.x, mPosition.y, 0));	// translate it
					Clear(eDirtyMatrix);
				}
				return mMatrix;
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f const &GetScale() const
			{
				return mScale;
			}

			//////////////////////////////////////////////////////////////////////

			Element &SetScale(Vec2f scale)
			{
				mScale = scale;
				Set(eDirtyMatrix);
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			float GetRotation() const
			{
				return mAngle;
			}

			//////////////////////////////////////////////////////////////////////

			Element &SetRotation(float angle)
			{
				mAngle = angle;
				Set(eDirtyMatrix);
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f LocalToScreen(Vec2f point) const
			{
				Vec4f p = TransformPoint(Vec4(point.x, point.y, 0, 1), mTransformMatrix);
				return Vec2f(GetX(p), GetY(p));
			}

			//////////////////////////////////////////////////////////////////////
			
			Vec2f ScreenToLocal(Vec2f point) const
			{
				Vec4f p = TransformPoint(Vec4(point.x, point.y, 0, 1), mInverseMatrix);
				return Vec2f(GetX(p), GetY(p));
			}

			//////////////////////////////////////////////////////////////////////

			bool ContainsLocalPoint(Vec2f point) const
			{
				return point >= Vec2f::zero && point < mSize;
			}

			//////////////////////////////////////////////////////////////////////

			bool ContainsScreenPoint(Vec2f point) const
			{
				return PointInRectangle(point, mScreenCoordinates);
			}

			//////////////////////////////////////////////////////////////////////

			virtual Element &AddChildAndCenter(Element &e)
			{
				return AddChild(e.SetPivot(Vec2f::half).SetPosition(GetSize() / 2));
			}

			//////////////////////////////////////////////////////////////////////

			virtual Element &AddChild(Element &e)
			{
				mChildren.push_back(e);
				e.mParent = this;
				Set(eReorderRequired);
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			virtual void RemoveChild(Element &e)
			{
				mChildren.remove(e);
				e.mParent = null;
			}

			//////////////////////////////////////////////////////////////////////

			Element &SetZIndex(int index)
			{
				mZIndex = index;
				if(mParent != null)
				{
					mParent->Set(eReorderRequired);
				}
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			int GetZIndex() const
			{
				return mZIndex;
			}

			//////////////////////////////////////////////////////////////////////

			void SortChildren()
			{
				if(Is(eReorderRequired))
				{
					mChildren.sort([] (Element const &a, Element const &b)
					{
						return a.mZIndex < b.mZIndex;
					});
					Clear(eReorderRequired);
				}
			}

			//////////////////////////////////////////////////////////////////////

			virtual bool IsClipper() const
			{
				return false;
			}

			//////////////////////////////////////////////////////////////////////

			virtual void OnDrawComplete(DrawList &drawList)
			{
			}

			//////////////////////////////////////////////////////////////////////

			virtual void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
			{
				// Empty UI Element doesn't draw anything, useful as a container for other elements
			}

			//////////////////////////////////////////////////////////////////////

			virtual char const *Name() const
			{
				return "Element";
			}

			//////////////////////////////////////////////////////////////////////
			// make a list of all messages which have arrived this frame and
			// process them on the root

			void Draw(ID3D11DeviceContext *context, DrawList &drawList, Matrix const &ortho, Element *clipper = null);

			// TODO (charlie): allow controls to SetCapture the mouse
			// TODO (charlie): keyboard focus

			// Process a message
			// Only the root UI element should have this function called on it...

			bool ProcessMessage(Message *m, bool clip);

			//////////////////////////////////////////////////////////////////////

			bool Overlaps(Element const &b);

			//////////////////////////////////////////////////////////////////////

			void UpdateTransform(Matrix const &matrix)
			{
				// parent can transform its children through mClientTransform - blunt instrument, though
				Matrix &cm = mParent != null ? mParent->mClientTransform : IdentityMatrix;
				mTransformMatrix = GetMatrix() * cm * matrix;
				mInverseMatrix = DirectX::XMMatrixInverse(null, mTransformMatrix);

				Vec2f s = GetSize();
				Vec2f p[4] = { Vec2f::zero, { s.x, 0 }, s, { 0, s.y } };
				for(uint i = 0; i < 4; ++i)
				{
					mScreenCoordinates[i] = LocalToScreen(p[i]);
					if(i == 0)
					{
						mScreenMax = mScreenCoordinates[i];
						mScreenMin = mScreenCoordinates[i];
					}
					else
					{
						mScreenMax = Max(mScreenMax, mScreenCoordinates[i]);
						mScreenMin = Min(mScreenMin, mScreenCoordinates[i]);
					}
				}
				if(true)
				{
//					debug_outline_quad2d(mScreenCoordinates, Is(eTransparent) ? Color::Magenta : Color::Cyan);
//					debug_line2d({ mScreenMin.x, 0 }, { mScreenMin.x, 1000 }, Color::Aquamarine);
//					debug_line2d({ mScreenMax.x, 0 }, { mScreenMax.x, 1000 }, Color::Aquamarine);
//					debug_line2d({ 0, mScreenMin.y }, { 1000, mScreenMin.y }, Color::Aquamarine);
//					debug_line2d({ 0, mScreenMax.y }, { 1000, mScreenMax.y }, Color::Aquamarine);
				}

				for(auto &r : mChildren)
				{
					((Element &)r).UpdateTransform(mTransformMatrix);
				}
			}

			//////////////////////////////////////////////////////////////////////
			// TODO (charlie): modality
			// TODO (charlie): make this private and UI::Update() a friend

			void Update()
			{
				if(!Is(eInActive))
				{
					Updating.Invoke(this);

					for(auto &r : mChildren)
					{
						((Element &)r).Update();
					}
				}
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct OutlineRectangle: Element
		{
			Color mColor;

			OutlineRectangle()
				: Element()
			{
				Set(eTransparent);
			}

			OutlineRectangle &SetColor(Color c)
			{
				mColor = c;
				return *this;
			}

			Color GetColor() const
			{
				return mColor;
			}

			char const *Name() const override
			{
				return "OutlineRectangle";
			}

			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override;
		};

		//////////////////////////////////////////////////////////////////////

		struct FilledRectangle: Element
		{
			Color mColor;

			FilledRectangle &SetColor(Color c)
			{
				mColor = c;
				return *this;
			}

			Color GetColor() const
			{
				return mColor;
			}

			char const *Name() const override
			{
				return "FilledRectangle";
			}

			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override;
		};

		//////////////////////////////////////////////////////////////////////

		struct ClipRectangle: Element
		{
			ClipRectangle()
				: Element()
			{
				Set(eTransparent);
			}

			bool IsClipper() const override;
			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override;
			void OnDrawComplete(DrawList &drawList) override;

			char const *Name() const override
			{
				return "ClipRectangle";
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct Rectangle: FilledRectangle
		{
			OutlineRectangle mOutlineRectangle;
			Delegate<UIEvent> mUpdatingDelegate;

			Rectangle()
				: FilledRectangle()
				, mUpdatingDelegate([this] (UIEvent const &e)
				{
					mOutlineRectangle.SetSize(GetSize());
				})
			{
				AddChild(mOutlineRectangle);
				mOutlineRectangle.SetColor(Color::White);
				Updating += mUpdatingDelegate;
			}

			Rectangle &SetLineColor(Color c)
			{
				mOutlineRectangle.SetColor(c);
				return *this;
			}

			Color GetLineColor() const
			{
				return mOutlineRectangle.GetColor();
			}

			char const *Name() const override
			{
				return "Rectangle";
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct Line: Element
		{
			Line()
				: Element()
			{
				Set(eTransparent);
			}

			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override;

			Color	mColor;

			Line &SetColor(Color c)
			{
				mColor = c;
				return *this;
			}

			Color GetColor() const
			{
				return mColor;
			}

			char const *Name() const override
			{
				return "Line";
			}
		};

		//////////////////////////////////////////////////////////////////////
		// need a font instance and vertexbuffer

		struct Label: Element
		{
			string		mText;	// UTF8
			uint32		mLayerMask;
			Typeface *	mTypeface;

			//////////////////////////////////////////////////////////////////////

			Label()
				: Element()
				, mLayerMask(0xffffffff)
				, mTypeface(null)
			{
			}

			char const *Name() const override
			{
				return "Label";
			}

			//////////////////////////////////////////////////////////////////////

			float Baseline() const
			{
				return mTypeface->GetBaseline() / mTypeface->GetHeight();
			}

			//////////////////////////////////////////////////////////////////////

			Label &SetLayerMask(uint32 mask)
			{
				mLayerMask = mask;
				return *this;
			}

			uint32 GetLayerMask() const
			{
				return mLayerMask;
			}

			//////////////////////////////////////////////////////////////////////

			void Measure()
			{
				if(!mText.empty() && mTypeface != null)
				{
					mSize = mTypeface->MeasureString(mText.c_str());
					float hb = mTypeface->GetBaseline() / mTypeface->GetHeight() / 2;
					SetPivot(Vec2f(0.5f, hb));
				}
			}

			//////////////////////////////////////////////////////////////////////

			Label &SetFont(Typeface *font)
			{
				mTypeface = font;
				Measure();
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			Label &SetText(char const *text)
			{
				mText = text;
				Measure();
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override;
		};

		//////////////////////////////////////////////////////////////////////
		// TODO (charlie): make it so it can use a sprite atlas instead of individual textures

		struct Image : Element
		{
			Texture *mGraphic;
			Sampler *mSampler;

			//////////////////////////////////////////////////////////////////////

			Image()
				: Element()
				, mGraphic(null)
				, mSampler(null)
			{
			}

			//////////////////////////////////////////////////////////////////////

			Image &SetImage(Texture *t)
			{
				mGraphic = t;
				mSize = t->FSize();
				return *this;
			}

			char const *Name() const override
			{
				return "Image";
			}

			//////////////////////////////////////////////////////////////////////

			Image &SetSampler(Sampler *s)
			{
				mSampler = s;
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override;

		};

		//////////////////////////////////////////////////////////////////////

		struct Button: Image
		{
			//////////////////////////////////////////////////////////////////////

			Button()
				: Image()
			{
				SetPivot(Vec2f(0.5f, 0.5f));
			}

			char const *Name() const override
			{
				return "Button";
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct LabelButton: Button
		{
			Label mLabel;

			//////////////////////////////////////////////////////////////////////

			LabelButton()
				: Button()
			{
				mLabel.Set(eTransparent);
				AddChild(mLabel);
			}

			char const *Name() const override
			{
				return "LabelButton";
			}

			//////////////////////////////////////////////////////////////////////

			Label &GetLabel()
			{
				return mLabel;
			}

			//////////////////////////////////////////////////////////////////////

			LabelButton &SetImage(Texture *t)
			{
				Button::SetImage(t);
				mLabel.SetPivot(Vec2f::half);
				mLabel.SetPosition(t->FSize() / 2);	// alignment option (topleft, bottomright etc)
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			LabelButton &SetFont(Typeface *f)
			{
				mLabel.SetFont(f);
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			LabelButton &SetText(char const *t)
			{
				mLabel.SetText(t);
				return *this;
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct ScrollBar: FilledRectangle
		{
			Delegate<MouseEvent>	mMouseMovedDelegate;
			Delegate<PressedEvent>	mMouseClickedDelegate;
			Delegate<ReleasedEvent>	mMouseReleasedDelgate;
			Delegate<MouseEvent>	mMouseEnteredDelegate;
			Delegate<MouseEvent>	mMouseLeftDelegate;
			bool					mDrag;
			Vec2f					mDragOffset;

			ScrollBar()
				: FilledRectangle()
				, mDrag(false)
			{
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
							MoveTo(e.mMousePosition.y - mDragOffset.y);
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

			void MoveTo(float position)
			{
				Element *p = mParent;
				float total = p->Height() - Height() - 1;
				SetPosition({ mPosition.x, Max(0.0f, Min(total, position)) });
				float ratio = mPosition.y / total;
				p->Messenger.Invoke(MessengerEvent(this, eScrolled, &ratio)); // notify the parent that they should scroll
			};

			void Move(float delta)
			{
				MoveTo(mPosition.y + delta);
			}

			char const *Name() const override
			{
				return "LabelButton";
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct ListBox: Element
		{
			FilledRectangle				mFilledRectangle;
			OutlineRectangle			mOutlineRectangle;
			ClipRectangle				mClipRectangle;
			int							mStringCount;
			Typeface *					mTypeface;
			ScrollBar					mScrollBar;
			FilledRectangle				mSelection;
			float						mScrollPosition;
			Delegate<MessengerEvent>	mMessengerDelegate;
			Delegate<MouseWheelEvent>	mMouseWheelDelegate;
			float						mScrollVelocity;
			Vec2f						mOrigin;
			Vec2f						mClientSize;

			ListBox()
				: Element()
				, mStringCount(0)
				, mTypeface(null)
				, mScrollPosition(0)
				, mOrigin(Vec2f::zero)
			{
				AddChild(mFilledRectangle);
				AddChild(mClipRectangle);
				AddChild(mScrollBar);
				AddChild(mOutlineRectangle);
				mFilledRectangle.SetColor(0x80000000).Set(eTransparent);
				mOutlineRectangle.SetColor(Color::White);
				mScrollBar.SetSize({ 8, 12 }).SetVisible(true).SetZIndex(1);

				Messenger += mMessengerDelegate = [this] (MessengerEvent const &e)
				{
					switch(e.msg)
					{
						case eScrolled:
						{
							float spill = ClientHeight() - Height();
							if(spill > 0)
							{
								mOrigin.y = floorf(spill * *((float *)e.data));
								mClipRectangle.mClientTransform = TranslationMatrix(Vec4(0, -mOrigin.y, 0));
							}
						}
						break;
					}
				};

				MouseWheeled += mMouseWheelDelegate = [this] (MouseWheelEvent const &e)
				{
					Scroll(-e.wheelDelta * RowHeight());
				};
			}

			void Scroll(float amount)
			{
				// change client offset by amount
				// update scrollbar position
				float spill = ClientHeight() - Height();
				if(spill > 0)
				{
					mOrigin.y = Min(spill, Max(0.0f, mOrigin.y + amount));
					float ratio = mOrigin.y / spill;	// relative offset
					float total = Height() - mScrollBar.Height() - 1;
					mScrollBar.SetPosition({ mScrollBar.mPosition.x, total * ratio });
					mClipRectangle.mClientTransform = TranslationMatrix(Vec4(0, -mOrigin.y, 0));
				}
			}

			char const *Name() const override
			{
				return "ListBox";
			}

			float RowHeight() const
			{
				return (mTypeface == null) ? 0 : (mTypeface->GetHeight() + 2.0f);
			}

			float ClientHeight() const
			{
				return mClientSize.y;
			}

			float ClientWidth() const
			{
				return mClientSize.x;
			}

			Vec2f const &ClientSize() const
			{
				return mClientSize;
			}

			void SetClientWidth(float w)
			{
				mClientSize.x = w;
			}

			void SetClientHeight(float h)
			{
				mClientSize.y = h;
				UpdateScrollbar();
			}

			void UpdateScrollbar()
			{
				float ch = ClientHeight();
				if(ch > 0)
				{
					float ratio = Height() / ch;				// eg 100 / 200 = 0.5
					if(ratio < 1)
					{
						float scrollBarHeight = Min(Height(), Max(8.0f, Height() * ratio));		// eg half the height of the window
						mScrollBar.SetSize({ 8, scrollBarHeight });
						mScrollBar.Show();
						return;
					}
				}
				mScrollBar.Hide();
			}

			ListBox &SetFont(Typeface *f)
			{
				mTypeface = f;
				SetClientHeight(mStringCount * RowHeight());
				return *this;
			}

			Element &SetSize(Vec2f size) override
			{
				Element::SetSize(size);
				mClipRectangle.SetSize(size);
				mOutlineRectangle.SetSize(size);
				mFilledRectangle.SetSize(size);
				mScrollBar.SetPosition({ size.x - mScrollBar.Width(), 0 });
				UpdateScrollbar();
				return *this;
			}

			Element *AddString(char const *text)
			{
				float tfh = (float)mTypeface->GetHeight() + 2.0f;
				FilledRectangle *r = new FilledRectangle();
				r->SetColor(Color::DarkGreen).SetPosition({ 0, (float)mStringCount * tfh }).SetSize({ mClipRectangle.Width(), tfh });

				r->MouseEntered += [] (MouseEvent const &m)
				{
					FilledRectangle *f = (FilledRectangle *)m.mElement;
					f->SetColor(Color::White);
				};

				r->MouseLeft += [] (MouseEvent const &m)
				{
					FilledRectangle *f = (FilledRectangle *)m.mElement;
					f->SetColor(Color::DarkGreen);
				};

				r->Updating += [r] (UIEvent const &e)
				{
					FilledRectangle *f = (FilledRectangle *)e.mElement;
				};

				mClipRectangle.AddChild(*r);
				Label *b = new Label();
				b->SetText(text).SetFont(mTypeface).SetPivot({ 0, 0 }).SetPosition({ 1, 1 }).Set(eTransparent);
				r->AddChild(*b);

				++mStringCount;
				SetClientHeight(RowHeight() * mStringCount);
				UpdateScrollbar();
				return b;
			}
		};

		inline bool MouseMessage::Pick(Element const *e) const
		{
			return e->ContainsScreenPoint(mPosition);
		}
	}
}
