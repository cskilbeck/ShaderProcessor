//////////////////////////////////////////////////////////////////////
// TODO (charlie): fix how janky and lame this is
// TODO (charlie): scrollbars
// TODO (charlie): cliprectangles
// TODO (charlie): listview
// TODO (charlie): input
// TODO (charlie): checkbox
// TODO (charlie): slider
// TODO (charlie): etc

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

		struct ElementBase: Aligned16, list_node<ElementBase>
		{
			Matrix			mMatrix;
			Matrix			mTransformMatrix;
			Matrix			mInverseMatrix;
			ElementBase *	mParent;
			int				mZIndex;
			Flags32			mFlags;
			Vec2f			mPosition;
			Vec2f			mSize;
			Vec2f			mScale;
			Vec2f			mPivot;
			float			mAngle;

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

			bool operator < (ElementBase &o)
			{
				return mZIndex < o.mZIndex;
			}

			ElementBase()
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
			{
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

			ElementBase &SetVisible(bool s)
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

			ElementBase &SetPosition(Vec2f pos)
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

			ElementBase &SetSize(Vec2f size)
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

			ElementBase &SetPivot(Vec2f pivot)
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
						ScaleMatrix(Vec4(mScale.x, mScale.y, 0)) *				// scale it
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

			ElementBase &SetScale(Vec2f scale)
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

			ElementBase &SetRotation(float angle)
			{
				mAngle = angle;
				Set(eDirtyMatrix);
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f LocalToScreen(Vec2f point)
			{
				Vec4f p = TransformPoint(Vec4(point.x, point.y, 0, 1), mTransformMatrix);
				return Vec2f(GetX(p), GetY(p));
			}

			//////////////////////////////////////////////////////////////////////
			
			Vec2f ScreenToLocal(Vec2f point)
			{
				Vec4f p = TransformPoint(Vec4(point.x, point.y, 0, 1), mInverseMatrix);
				return Vec2f(GetX(p), GetY(p));
			}

			//////////////////////////////////////////////////////////////////////

			bool ContainsLocalPoint(Vec2f point)
			{
				return point >= Vec2f::zero && point < mSize;
			}

			//////////////////////////////////////////////////////////////////////

			bool ContainsScreenPoint(Vec2f point)
			{
				return ContainsLocalPoint(ScreenToLocal(point));
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct Element: ElementBase
		{
			linked_list<ElementBase>	mChildren;

			Event<ClickedEvent>			Clicked;
			Event<PressedEvent>			Pressed;
			Event<ReleasedEvent>		Released;
			Event<UIEvent>				Updating;
			Event<UIEvent>				Updated;
			Event<MouseEvent>			MouseEntered;
			Event<MouseEvent>			MouseLeft;
			Event<MouseEvent>			Hovering;

			//////////////////////////////////////////////////////////////////////

			Element()
				: ElementBase()
			{
			}

			//////////////////////////////////////////////////////////////////////

			Element &AddChild(Element &e)
			{
				mChildren.push_back((ElementBase &)e);
				e.mParent = this;
				Set(eReorder);
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			void RemoveChild(Element &e)
			{
				mChildren.remove((ElementBase &)e);
				e.mParent = null;
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

			virtual void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
			{
				// Empty UI Element doesn't draw anything, useful as a container for other elements
			}

			//////////////////////////////////////////////////////////////////////

			virtual void OnUpdate(float deltaTime)
			{
			}

			//////////////////////////////////////////////////////////////////////

			void Draw(ID3D11DeviceContext *context, DrawList &drawList, Matrix const &ortho);

			//////////////////////////////////////////////////////////////////////
			// TODO (charlie): modality

			void Update(float deltaTime, Matrix const &matrix)
			{
				// matrices are always updated even if it's inactive
				mTransformMatrix = GetMatrix() * matrix;
				mInverseMatrix = DirectX::XMMatrixInverse(null, mTransformMatrix);

				// clicking & onupdate only called if it's active
				if(!Is(eInActive))
				{
					Vec2f local = ScreenToLocal(Mouse::Position);
					if(ContainsLocalPoint(local))
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
					OnUpdate(deltaTime);
					Updated.Invoke(this);
				}

				// children processed always (to keep matrices up  to date)
				for(auto &r : mChildren)
				{
					((Element &)r).Update(deltaTime, mTransformMatrix);
				}
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct Rectangle: Element
		{
			Color mColor;

			Rectangle &SetColor(Color c)
			{
				mColor = c;
				return *this;
			}

			Color GetColor() const
			{
				return mColor;
			}

			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override;
		};

		//////////////////////////////////////////////////////////////////////

		struct ClipRectangle: Element
		{
			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override;
		};

		//////////////////////////////////////////////////////////////////////
		// need a font instance and vertexbuffer

		struct Label: Element
		{
			string mText;	// UTF8
			Typeface *mTypeface;
			Font mFont;

			//////////////////////////////////////////////////////////////////////

			Label()
				: Element()
				, mTypeface(null)
			{
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

			//////////////////////////////////////////////////////////////////////

			void OnUpdate(float deltaTime) override
			{
				Element::OnUpdate(deltaTime);
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct LabelButton: Button
		{
			Label label;

			//////////////////////////////////////////////////////////////////////

			LabelButton()
				: Button()
			{
				AddChild(label);
			}

			//////////////////////////////////////////////////////////////////////

			LabelButton &SetImage(Texture *t)
			{
				Button::SetImage(t);
				label.SetPosition(t->FSize() / 2);	// alignment option (topleft, bottomright etc)
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			LabelButton &SetFont(Typeface *f)
			{
				label.SetFont(f);
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			LabelButton &SetText(char const *t)
			{
				label.SetText(t);
				return *this;
			}
		};
	}
}
