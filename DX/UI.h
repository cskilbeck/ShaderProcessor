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

// Window: ClipRectangle, [ScrollBars], {ClientSize, ClientPos}

// TODO (charlie): event suppression/bubbling

// NOTE: cliprects do not 'stack' or combine. Each cliprect sets the 4 clip planes independently of any parent clip plane...

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

		//////////////////////////////////////////////////////////////////////
		// A Mouse or Keyboard (or whatever) message

		struct Message: list_node
		{
			enum Type
			{
				MouseMove = 0,
				MouseLeftButtonDown = 1,
				MouseLeftButtonUp = 2,
				MouseRightButtonDown = 3,
				MouseRightButtonUp = 4,
				KeyPress = 5,
				KeyRelease = 6
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

		void Open(Window *w);
		MouseMessage *AddMouseMessage(Message::Type type, Vec2f const &point);
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

			// Events

			Event<ClickedEvent>		Clicked;
			Event<PressedEvent>		Pressed;
			Event<ReleasedEvent>	Released;
			Event<UIEvent>			Updating;
			Event<MouseEvent>		MouseMoved;
			Event<MouseEvent>		MouseEntered;
			Event<MouseEvent>		MouseLeft;
			Event<MouseEvent>		Hovering;

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
				v ? Set(f) : Clear(f);
				return *this;
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
				SetFlag(eHidden, !s);
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			void Show()
			{
				SetVisible(true);
			}

			//////////////////////////////////////////////////////////////////////

			void Hide()
			{
				SetVisible(false);
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
				return ContainsLocalPoint(ScreenToLocal(point));
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

			void Draw(ID3D11DeviceContext *context, DrawList &drawList, Matrix const &ortho);

			// TODO (charlie): allow controls to SetCapture the mouse
			// TODO (charlie): keyboard focus

			// Process a message
			// Only the root UI element should have this function called on it...

			bool ProcessMessage(Message *m);

			//////////////////////////////////////////////////////////////////////

			void UpdateTransform(Matrix const &matrix)
			{
				// parent can transform its children through mClientTransform - blunt instrument, though
				Matrix &cm = mParent != null ? mParent->mClientTransform : IdentityMatrix;
				mTransformMatrix = GetMatrix() * cm * matrix;
				mInverseMatrix = DirectX::XMMatrixInverse(null, mTransformMatrix);

				for(auto &r : mChildren)
				{
					((Element &)r).UpdateTransform(mTransformMatrix);
				}
			}

			//////////////////////////////////////////////////////////////////////
			// TODO (charlie): modality
			// TODO (charlie): make this private and UI::Update() a friend

			void Update(float deltaTime, bool clip, bool active)
			{
				return;
				// SOMEHOW: clip on the way down, but invoke messages on the way up
				// -- set flags (eHovering, ePressed etc) on the way down
				// -- use those flags on the way up to invoke messages
				// -- build message list (only child-most gets each message), send to child-most, and parents until bubble = false

				active &= Is(eInActive);

				// clicking & onupdate only called if it's active (and not clipped)
				if(!active)
				{
					Vec2f local = ScreenToLocal(Mouse::Position);
					if(ContainsLocalPoint(local) && !clip)
					{
						if(!Is(eHovering))
						{
							MouseEntered.Invoke(MouseEvent(this, local));
							Set(eHovering);
						}
						Hovering.Invoke(MouseEvent(this, local));
						if(Mouse::Pressed & Mouse::Button::Left)
						{
							Set(ePressed);
							Pressed.Invoke(PressedEvent(this, local));
						}
						if(Mouse::Released & Mouse::Button::Left && Is(ePressed))
						{
							Clear(ePressed);
							Released.Invoke(ReleasedEvent(this, local));
						}
					}
					else
					{
						clip |= IsClipper();

						if(Is(eHovering))
						{
							MouseLeft.Invoke(MouseEvent(this, local));
							Clear(eHovering);
						}
						if(Is(ePressed))
						{
							Clear(ePressed);
							Released.Invoke(ReleasedEvent(this, local));
						}
					}
					Updating.Invoke(this);
				}
				// children always processed (to keep matrices up  to date)
				for(auto &r : mChildren)
				{
					((Element &)r).Update(deltaTime, clip, active);
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
			
			Rectangle()
				: FilledRectangle()
			{
				AddChild(mOutlineRectangle);
				mOutlineRectangle.SetColor(Color::White);
				Updating += [this] (UIEvent const &e)
				{
					mOutlineRectangle.SetSize(GetSize());
				};
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

		struct ListBox: Element
		{
			FilledRectangle		mFilledRectangle;
			OutlineRectangle	mOutlineRectangle;
			ClipRectangle		mClipRectangle;
			int					mStringCount;
			Typeface *			mTypeface;
			FilledRectangle		mScrollBar;
			FilledRectangle		mSelection;
			float				mScrollPosition;

			ListBox()
				: Element()
				, mStringCount(0)
				, mTypeface(null)
				, mScrollPosition(0)
			{
				AddChild(mFilledRectangle);
				AddChild(mClipRectangle);
				AddChild(mScrollBar);
				AddChild(mOutlineRectangle);
				mFilledRectangle.Set(eTransparent);
				mFilledRectangle.SetColor(0x80000000);
				mOutlineRectangle.SetColor(Color::White);
				mScrollBar.SetColor(0x80ffffff);
				mScrollBar.SetSize({ 8, 12 });
				mScrollBar.SetVisible(true);
			}

			char const *Name() const override
			{
				return "ListBox";
			}

			void UpdateScrollbar()
			{
				if(mStringCount == 0)
				{
					return;
				}

				float textHeight = (float)mStringCount * mTypeface->GetHeight();
				float ratio = GetSize().y / textHeight;
				float scrollBarHeight = GetSize().y * ratio;
				mScrollBar.SetSize({ 8, scrollBarHeight });
			}

			ListBox &SetFont(Typeface *f)
			{
				mTypeface = f;
				UpdateScrollbar();
				return *this;
			}

			Element &SetSize(Vec2f size) override
			{
				Element::SetSize(size);
				mClipRectangle.SetSize(size);
				mOutlineRectangle.SetSize(size);
				mFilledRectangle.SetSize(size);
				mScrollBar.SetPosition({ size.x - mScrollBar.Width() - 2, 2 });
				UpdateScrollbar();
				return *this;
			}

			Element *AddString(char const *text)
			{
				Label *b = new Label();
				b->SetText(text);
				b->SetPosition({ 0, (float)mStringCount * mTypeface->GetHeight() });
				b->SetFont(mTypeface);
				b->SetPivot({ 0, 0 });	// override centering behaviour by setting pivot last
				++mStringCount;
				mClipRectangle.AddChild(*b);
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
