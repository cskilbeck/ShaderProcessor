//////////////////////////////////////////////////////////////////////
// TODO (charlie): fix how janky and lame this is
// TODO (charlie): scrollbars
// TODO (charlie): cliprectangles
// TODO (charlie): horizontal/verticalline
// TODO (charlie): listview
// TODO (charlie): input
// TODO (charlie): checkbox
// TODO (charlie): slider
// TODO (charlie): icons (X, _, etc) ?
// TODO (charlie): etc

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

		//////////////////////////////////////////////////////////////////////

		void Open();
		void Update(Element *rootElement, float deltaTime);
		void Draw(Element *rootElement, ID3D11DeviceContext *context, DrawList &drawList, Matrix const &ortho);
		void Close();

		//////////////////////////////////////////////////////////////////////

		struct UIEvent
		{
			Element *mElement;

			UIEvent(Element *element)
				: mElement(element)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct MouseEvent: UIEvent
		{
			Vec2f mMousePosition;

			MouseEvent(Element *element, Vec2f mousePosition)
				: UIEvent(element)
				, mMousePosition(mousePosition)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct ClickedEvent: MouseEvent
		{
			ClickedEvent(Element *element, Vec2f mousePosition)
				: MouseEvent(element, mousePosition)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct PressedEvent: MouseEvent
		{
			PressedEvent(Element *element, Vec2f mousePosition)
				: MouseEvent(element, mousePosition)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct ReleasedEvent: MouseEvent
		{
			ReleasedEvent(Element *element, Vec2f mousePosition)
				: MouseEvent(element, mousePosition)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct Element: Aligned16, chs::list_node
		{
			Element *				mParent;
			chs::linked_list<Element>	mChildren;

			Matrix					mMatrix;
			Matrix					mTransformMatrix;
			Matrix					mInverseMatrix;
			Matrix					mClientTransform;

			int						mZIndex;
			Flags32					mFlags;

			Vec2f					mPosition;
			Vec2f					mSize;
			Vec2f					mScale;
			Vec2f					mPivot;
			float					mAngle;

			Event<ClickedEvent>		Clicked;
			Event<PressedEvent>		Pressed;
			Event<ReleasedEvent>	Released;
			Event<UIEvent>			Updating;
			Event<MouseEvent>		MouseEntered;
			Event<MouseEvent>		MouseLeft;
			Event<MouseEvent>		Hovering;

			enum: uint32
			{
				eHidden = 1,		// draw it or don't
				eInActive = 2,		// call OnUpdate() or don't
				eEnabled = 4,		// process input messages or don't
				eClosed = 8,		// close requested, will happen at the end of the frame
				eModal = 16,		// block parental input
				eReorder = 32,		// child was added, sort children
				eDirtyMatrix = 64,	// Matrix needs to be rebuilt
				eHovering = 128,	// Mouse is currently hovering over the Element
				ePressed = 256		// Mouse button is pressed down on the Element
			};

			bool operator > (Element &o)
			{
				return mZIndex > o.mZIndex;
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

			void Set(uint32 f)
			{
				mFlags.Set(f);
			}

			//////////////////////////////////////////////////////////////////////

			void Clear(uint32 f)
			{
				mFlags.Clear(f);
			}

			//////////////////////////////////////////////////////////////////////

			void SetFlag(uint32 f, bool v = true)
			{
				v ? Set(f) : Clear(f);
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

			Element &SetSize(Vec2f size)
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

			Vec2f GetScale() const
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
				Set(eReorder);
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
					mParent->Set(eReorder);
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
				if(Is(eReorder))
				{
					mChildren.sort();
					Clear(eReorder);
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

			void Draw(ID3D11DeviceContext *context, DrawList &drawList, Matrix const &ortho);

			//////////////////////////////////////////////////////////////////////
			// TODO (charlie): modality
			// TODO (charlie): make this private and UI::Update() a friend

			void Update(float deltaTime, Matrix const &matrix, bool clip, bool active)
			{
				// parent can transform its children through mClientTransform - blunt instrument, though
				Matrix &cm = mParent != null ? mParent->mClientTransform : IdentityMatrix;

				// matrices are always updated even if it's inactive because it might be visible
				mTransformMatrix = GetMatrix() * cm * matrix;
				mInverseMatrix = DirectX::XMMatrixInverse(null, mTransformMatrix);

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
					((Element &)r).Update(deltaTime, mTransformMatrix, clip, active);
				}
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct OutlineRectangle: Element
		{
			Color mColor;

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
				return "OutlineRectangle";
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

		// how to mess with the position of the client children?

		struct Scrollable: Element
		{
			ClipRectangle		mClient;
			FilledRectangle		mVerticalScrollBar;
			FilledRectangle		mHorizontalScrollBar;

			Scrollable()
				: Element()
			{
				AddChild(mClient);
				AddChild(mVerticalScrollBar);
				AddChild(mHorizontalScrollBar);
				mVerticalScrollBar.SetColor(Color::White);
				mHorizontalScrollBar.SetColor(Color::White);

				Updating += [this] (UIEvent const &e)
				{
					// work out extents of the children
					Vec2f tl(FLT_MAX, FLT_MAX);
					Vec2f br(-FLT_MAX, -FLT_MAX);
					for(auto &c : mClient.mChildren)
					{
						Vec2f s = c.GetSize();
						Vec2f corners[4] = { Vec2f::zero, s, Vec2f(0, s.y), Vec2f(s.x, 0) };
						for(auto &corner : corners)
						{
							Vec2f cr = c.LocalToScreen(corner);
							tl = Min(tl, cr);
							br = Max(br, cr);
						}
					}
					// work out scrollbar sizes
					Vec2f mySize = GetSize();
					Vec2f childrenSize = br - tl;						// eg 200
					Vec2f ratio = mySize / childrenSize;				// eg 100 / 200 = 0.5
					Vec2f sbSize = Max(mySize * ratio, Vec2f(16, 16));	// eg 100 * 0.5 = 50
					sbSize = Min(GetSize(), sbSize);					// shrink if they don't fit in the window

					mVerticalScrollBar.SetSize(Vec2f(16, sbSize.y));
					mHorizontalScrollBar.SetSize(Vec2f(sbSize.x, 16));
				};
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
	}
}
