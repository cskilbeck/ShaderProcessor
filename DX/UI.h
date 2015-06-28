//////////////////////////////////////////////////////////////////////
// TODO (charlie): fix how janky and lame this is
// TODO (charlie): horizontal/verticalline
// TODO (charlie): input
// TODO (charlie): checkbox
// TODO (charlie): roundedRectangle
// TODO (charlie): slider
// TODO (charlie): icons (X, _, etc) ?

// TODO (charlie): event suppression/bubbling

// DONE (charlie): changing transform might change whether the stationary mouse is over the control
// DONE (charlie): scrollbars
// DONE (charlie): cliprectangles
// DONE (charlie): listview

// DROP (charlie): rounded rectangle? Can't do a roundedClipRect in just 8 clip planes... perhaps a stencil based clip mask thingy?

// Window: ClipRectangle, [ScrollBars], {ClientSize, ClientPos}
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
		struct MouseButtonMessage;
		struct MouseMoveMessage;
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
				MouseRightButtonUp,
				MouseDummy,

				KeyPress,
				KeyRelease,
				Messager
			};

			Type		mType;

			virtual bool IsMouseMessage() const
			{
				return false;
			}
		};

		// All mouse messages derive from this (or are this)

		struct MouseMessage: Message
		{
			Vec2f		mPosition;

			bool IsMouseMessage() const override
			{
				return true;
			}
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

		struct ControlEvent: UIEvent
		{
			Element *mChild;

			ControlEvent(Element *element, Element *child)
				: UIEvent(element)
				, mChild(child)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct DrawEvent: UIEvent
		{
			DrawEvent(Element *e)
				: UIEvent(e)
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
			float					mPickMargins[4];
			Vec2f					mScreenMin;
			Vec2f					mScreenMax;

			// Events

			Event<DrawEvent>		Drawing;
			Event<UIEvent>			Closing;
			Event<UIEvent>			Closed;
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
			Event<ControlEvent>		ChildAdded;
			Event<ControlEvent>		ChildRemoved;

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
				eTransparent = 512,		// Mouse events pass through it
				eSelected = 1024		// Mouse pressed on it and not moved off or released
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
				memset(mPickMargins, 0, sizeof(mPickMargins));
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

			void Select()
			{
				if(!Is(eSelected))
				{
//					Selected.Invoke(UIEvent(this));
					Set(eSelected);
				}
			}

			//////////////////////////////////////////////////////////////////////

			void DeSelect()
			{
				if(Is(eSelected))
				{
//					DeSelected.Invoke(UIEvent(this));
					Clear(eSelected);
				}
			}

			//////////////////////////////////////////////////////////////////////

			void Close()
			{
				if(!Is(eClosed))
				{
					Set(eClosed);
					Closing.Invoke(UIEvent(this));
				}
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

			void SetVisible(bool s)
			{
				SetFlag(eHidden, !s);
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

			void SetPickMargins(float left, float top, float right, float bottom)
			{
				mPickMargins[0] = left;
				mPickMargins[1] = top;
				mPickMargins[2] = right;
				mPickMargins[3] = bottom;
			}

			//////////////////////////////////////////////////////////////////////

			virtual bool Pick(Message const *m) const
			{
				if(m->IsMouseMessage())
				{
					MouseMessage *mm = (MouseMessage *)m;
					return ContainsScreenPoint(mm->mPosition);
				}
				else
				{
					return false;
				}
			}

			//////////////////////////////////////////////////////////////////////

			float const *GetMargins() const
			{
				return mPickMargins;
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f const &GetPosition() const
			{
				return mPosition;
			}

			//////////////////////////////////////////////////////////////////////

			void SetX(float x)
			{
				SetPosition({ x, mPosition.y });
			}

			//////////////////////////////////////////////////////////////////////

			void SetY(float y)
			{
				SetPosition({ mPosition.x, y });
			}

			//////////////////////////////////////////////////////////////////////

			void SetPosition(Vec2f pos)
			{
				mPosition = pos;
				Set(eDirtyMatrix);
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f const &GetSize() const
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

			void SetWidth(float width)
			{
				SetSize({ width, mSize.y });
			}

			//////////////////////////////////////////////////////////////////////

			void SetHeight(float height)
			{
				SetSize({ mSize.x, height });
			}

			//////////////////////////////////////////////////////////////////////

			virtual void SetSize(Vec2f const &size)
			{
				mSize = size;
				Set(eDirtyMatrix);
				Resized.Invoke(UIEvent(this));
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f GetPivot() const
			{
				return mPivot;
			}

			//////////////////////////////////////////////////////////////////////

			void SetPivot(Vec2f pivot)
			{
				mPivot = pivot;
				Set(eDirtyMatrix);
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

			void SetScale(Vec2f scale)
			{
				mScale = scale;
				Set(eDirtyMatrix);
			}

			//////////////////////////////////////////////////////////////////////

			float GetRotation() const
			{
				return mAngle;
			}

			//////////////////////////////////////////////////////////////////////

			void SetRotation(float angle)
			{
				mAngle = angle;
				Set(eDirtyMatrix);
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f LocalToScreen(Vec2f point) const
			{
				v4 p = TransformPoint(point, mTransformMatrix);
				return { p.x, p.y };
			}

			//////////////////////////////////////////////////////////////////////
			
			Vec2f ScreenToLocal(Vec2f point) const
			{
				v4 p = TransformPoint(point, mInverseMatrix);
				return { p.x, p.y };
			}

			//////////////////////////////////////////////////////////////////////

			bool ContainsLocalPoint(Vec2f point) const
			{
				return point >= Vec2f::zero && point < mSize;
			}

			//////////////////////////////////////////////////////////////////////

			bool ContainsScreenPointWithMargins(Vec2f point) const
			{
				return PointInRectangle(point, mScreenCoordinates, mPickMargins);
			}

			//////////////////////////////////////////////////////////////////////

			bool ContainsScreenPoint(Vec2f point) const
			{
				return PointInRectangle(point, mScreenCoordinates);
			}

			//////////////////////////////////////////////////////////////////////

			virtual void AddChildAndCenter(Element &e)
			{
				e.SetPosition(GetSize() / 2);
				e.SetPivot(Vec2f::half);
				AddChild(e);
			}

			//////////////////////////////////////////////////////////////////////

			virtual void AddChild(Element &e)
			{
				mChildren.push_back(e);
				e.mParent = this;
				Set(eReorderRequired);
				ChildAdded.Invoke(ControlEvent(this, &e));
			}

			//////////////////////////////////////////////////////////////////////

			virtual void RemoveChild(Element &e)
			{
				mChildren.remove(e);
				e.mParent = null;
				ChildRemoved.Invoke(ControlEvent(this, &e));
			}

			//////////////////////////////////////////////////////////////////////

			void SetZIndex(int index)
			{
				mZIndex = index;
				if(mParent != null)
				{
					mParent->Set(eReorderRequired);
				}
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

			virtual void OnUpdate(float deltaTime)
			{
			}

			//////////////////////////////////////////////////////////////////////

			virtual void OnUpdated(float deltaTime)
			{
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
				if(mParent != null)
				{
					mTransformMatrix = GetMatrix() * mParent->mClientTransform * matrix;
				}
				else
				{
					mTransformMatrix = GetMatrix() * matrix;
				}
				mInverseMatrix = DirectX::XMMatrixInverse(null, mTransformMatrix);

				Vec2f const &s = GetSize();
				Vec2f p[4] = { Vec2f::zero, { s.x, 0 }, s, { 0, s.y } };

				mScreenMin = mScreenMax = mScreenCoordinates[0] = LocalToScreen(p[0]);

				for(uint i = 1; i < 4; ++i)
				{
					mScreenCoordinates[i] = LocalToScreen(p[i]);
					mScreenMax = Max(mScreenMax, mScreenCoordinates[i]);
					mScreenMin = Min(mScreenMin, mScreenCoordinates[i]);
				}

				for(auto &r : mChildren)
				{
					((Element &)r).UpdateTransform(mTransformMatrix);
				}
			}

			//////////////////////////////////////////////////////////////////////
			// TODO (charlie): modality
			// TODO (charlie): make this private and UI::Update() a friend

			void Update(float deltaTime)
			{
				if(Is(eClosed))
				{
					mParent->RemoveChild(*this);
					Closed.Invoke(UIEvent(this));
				}
				else if(!Is(eInActive))
				{
					Updating.Invoke(this);
					OnUpdate(deltaTime);

					for(auto &r : mChildren)
					{
						((Element &)r).Update(deltaTime);
					}
					OnUpdated(deltaTime);
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

			void SetColor(Color c)
			{
				mColor = c;
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

			void SetColor(Color c)
			{
				mColor = c;
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
			Delegate<UIEvent> mResizedDelegate;

			Rectangle()
				: FilledRectangle()
			{
				AddChild(mOutlineRectangle);
				mOutlineRectangle.SetColor(Color::White);

				mResizedDelegate = [this] (UIEvent const &e)
				{
					mOutlineRectangle.SetSize(GetSize());
				};
				Resized += mResizedDelegate;
			}

			void SetLineColor(Color c)
			{
				mOutlineRectangle.SetColor(c);
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
		// TODO (charlie): enable textured shape (use Size.x & y for texture extents)

		struct Shape: Element
		{
			vector<Vec2f>	mPoints;
			Color			mColor;
			Flags32			mShapeFlags;

			enum
			{
				ePointsChanged = 1,
				eIsConvex = 2
			};

			Shape()
				: mShapeFlags(0)
			{
			}

			void AddPoint(Vec2f const &p)
			{
				mPoints.push_back(p);
				mShapeFlags.Set(ePointsChanged);
			}

			uint NumPoints() const
			{
				return (uint)mPoints.size();
			}

			void SetColor(Color c)
			{
				mColor = c;
			}

			Color GetColor() const
			{
				return mColor;
			}

			virtual void ClearPoints()
			{
				mPoints.clear();
			}

			void GetConvexity()
			{
				mShapeFlags.Set(eIsConvex, IsShapeConvex(mPoints.data(), (uint)mPoints.size()));
			}

			// Changes Position, Size and Pivot

			void ShrinkToFit()
			{
				if(!mPoints.empty())
				{
					Vec2f const &pos = GetPosition();
					Vec2f mn = mPoints[0] + pos;
					Vec2f mx = mPoints[0] + pos;
					for(uint i = 1; i < mPoints.size(); ++i)
					{
						mn = Min(mPoints[i] + pos, mn);
						mx = Max(mPoints[i] + pos, mx);
					}
					SetPosition(mn);
					SetSize(mx - mn);
					SetPivot((pos - mn) / GetSize());
				}
			}

			virtual void UpdateShape()
			{
				if(mShapeFlags(ePointsChanged))
				{
					if(mPoints.size() > 2)
					{
						mShapeFlags.Clear(ePointsChanged);
						GetConvexity();
					}
				}
			}

			bool Pick(Message const *m) const override
			{
				if(NumPoints() > 2 && m->IsMouseMessage())
				{
					MouseMessage const *mm = (MouseMessage const *)m;
					Vec2f sp = ScreenToLocal(mm->mPosition);
					if(mShapeFlags(eIsConvex))
					{
						return PointInConvexShape(mPoints.data(), (uint)mPoints.size(), sp);
					}
					else
					{
						return PointInConcaveShape(mPoints.data(), (uint)mPoints.size(), sp);
					}
				}
				else
				{
					return false;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct FilledShape: Shape
		{
			vector<uint16>	mIndices;
			uint			mNumTriangles;

			FilledShape()
				: Shape()
				, mNumTriangles(0)
			{
			}

			void ClearPoints() override
			{
				Shape::ClearPoints();
				mIndices.clear();
			}

			void UpdateShape() override
			{
				if(mPoints.size() > 2)
				{
					if(mShapeFlags(ePointsChanged))
					{
						mShapeFlags.Clear(ePointsChanged);
						GetConvexity();
						mIndices.resize((mPoints.size() - 2) * 3);
						if(mShapeFlags(eIsConvex))
						{
							mNumTriangles = TriangulateConvexPolygon(mPoints.data(), (uint)mPoints.size(), mIndices.data());
						}
						else
						{
							mNumTriangles = TriangulateConcavePolygon(mPoints.data(), (uint)mPoints.size(), mIndices.data());
						}
					}
				}
				else
				{
					mNumTriangles = 0;
				}
			}

			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override;
		};

		//////////////////////////////////////////////////////////////////////

		struct OutlineShape: Shape
		{
			OutlineShape()
				: Shape()
			{
				Set(eTransparent);
			}

			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override;
		};

		//////////////////////////////////////////////////////////////////////

		struct Triangle: FilledShape
		{
			Color		mOutlineColor;

			Triangle()
				: FilledShape()
			{
				float f = sqrtf(2);
				AddPoint({ 0, 0 });
				AddPoint({ f, f });
				AddPoint({ 0, 1 });
				UpdateShape();
			}

			void SetLineColor(Color c)
			{
				mOutlineColor = c;
			}

			Color GetLineColor() const
			{
				return mOutlineColor;
			}

			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override;
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

			void SetColor(Color c)
			{
				mColor = c;
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

		struct Label: FilledRectangle
		{
			string		mText;	// UTF8
			uint32		mLayerMask;
			Typeface *	mTypeface;
			Vec2f		mTextDimensions;
			Vec2f		mOffset;

			//////////////////////////////////////////////////////////////////////

			Label()
				: FilledRectangle()
				, mLayerMask(0xffffffff)
				, mTypeface(null)
				, mOffset(0,0)
			{
				mColor = 0;
			}

			//////////////////////////////////////////////////////////////////////

			void SetOffset(Vec2f const &o)
			{
				mOffset = o;
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f const &GetOffset() const
			{
				return mOffset;
			}

			//////////////////////////////////////////////////////////////////////

			char const *Name() const override
			{
				return "Label";
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f const &GetTextSize() const
			{
				if(mTypeface != null && !mText.empty())
				{
					return mTextDimensions;
				}
				return Vec2f::zero;
			}

			//////////////////////////////////////////////////////////////////////

			float Baseline() const
			{
				return mTypeface->GetBaseline() / mTypeface->GetHeight();
			}

			//////////////////////////////////////////////////////////////////////

			void SetLayerMask(uint32 mask)
			{
				mLayerMask = mask;
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
					mSize = mTextDimensions = mTypeface->MeasureString(mText.c_str());
					float hb = mTypeface->GetBaseline() / mTypeface->GetHeight() / 2;
					SetPivot(Vec2f(0.5f, hb));
				}
			}

			//////////////////////////////////////////////////////////////////////

			void SetFont(Typeface *font)
			{
				mTypeface = font;
				Measure();
			}

			//////////////////////////////////////////////////////////////////////

			void SetText(char const *text)
			{
				mText = text;
				Measure();
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

			void SetImage(Texture *t)
			{
				mGraphic = t;
				mSize = t->FSize();
			}

			char const *Name() const override
			{
				return "Image";
			}

			//////////////////////////////////////////////////////////////////////

			void SetSampler(Sampler *s)
			{
				mSampler = s;
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

			void SetImage(Texture *t)
			{
				Button::SetImage(t);
				mLabel.SetPivot(Vec2f::half);
				mLabel.SetPosition(t->FSize() / 2);	// alignment option (topleft, bottomright etc)
			}

			//////////////////////////////////////////////////////////////////////

			void SetFont(Typeface *f)
			{
				mLabel.SetFont(f);
			}

			//////////////////////////////////////////////////////////////////////

			void SetText(char const *t)
			{
				mLabel.SetText(t);
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct ScrollBar: FilledRectangle
		{
			enum Orientation
			{
				Vertical = 0,
				Horizontal = 1
			};
			Delegate<MouseEvent>	mMouseMovedDelegate;
			Delegate<PressedEvent>	mMouseClickedDelegate;
			Delegate<ReleasedEvent>	mMouseReleasedDelgate;
			Delegate<MouseEvent>	mMouseEnteredDelegate;
			Delegate<MouseEvent>	mMouseLeftDelegate;
			Vec2f					mDragOffset;
			Orientation				mOrientation;
			bool					mDrag;

			ScrollBar(Orientation orientation);

			void MoveTo(Vec2f const &position);
			void Move(Vec2f const &delta);

			char const *Name() const override
			{
				return "LabelButton";
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct Window: FilledRectangle
		{
			ClipRectangle				mClipRectangle;			// clipper
			ScrollBar					mVerticalScrollBar;		// scrollbars
			ScrollBar					mHorizontalScrollBar;
			Vec2f						mOrigin;				// client view offset
			Vec2f						mClientSize;			// client area
			Vec2f						mScrollTarget;

			//////////////////////////////////////////////////////////////////////

			Window()
				: FilledRectangle()
				, mOrigin(Vec2f::zero)
				, mClientSize(Vec2f::zero)
				, mVerticalScrollBar(ScrollBar::Vertical)
				, mHorizontalScrollBar(ScrollBar::Horizontal)
			{
				AddChild(mClipRectangle);
				AddChild(mVerticalScrollBar);
				AddChild(mHorizontalScrollBar);
				SetColor(0x80000000);
				mVerticalScrollBar.SetSize({ 8, 8 });
				mHorizontalScrollBar.SetSize({ 8, 8 });
				mVerticalScrollBar.Hide();
				mVerticalScrollBar.Hide();
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f SetScrollTarget(Vec2f const &t)
			{
				mScrollTarget = t;
				return mScrollTarget;
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f AddToScrollTarget(Vec2f const &t)
			{
				Vec2f o = mScrollTarget;
				SetScrollTarget(mScrollTarget + t);
				return mScrollTarget - o;
			}

			//////////////////////////////////////////////////////////////////////

			void OnUpdate(float deltaTime)
			{
				Vec2f d(mScrollTarget - mOrigin);
				if(d.x != 0 || d.y != 0)
				{
					if(d.Length() < 2)
					{
						mOrigin = mScrollTarget;
					}
					else
					{
						Vec2f o = mOrigin + d * Min(1.0f, deltaTime * 8);
						Vec2f s(ClientSize() - GetSize());
						Vec2f p = mOrigin;
						mOrigin = Max(Vec2f::zero, Min(o, s));
						Vec2f delta = p - mOrigin;
						if(delta.y == 0)
						{
							mScrollTarget.y = mOrigin.y;
						}
						if(delta.x == 0)
						{
							mScrollTarget.x = mOrigin.x;
						}
					}
					mClipRectangle.mClientTransform = TranslationMatrix(Vec4(floorf(-mOrigin.x), floorf(-mOrigin.y), 0));
					UpdateScrollbars();
				}
			}

			//////////////////////////////////////////////////////////////////////

			char const *Name() const override
			{
				return "Window";
			}

			//////////////////////////////////////////////////////////////////////
			// returns the amount that was added - 0 means it was already at an edge

			Vec2f Scroll(Vec2f const &amount)
			{
				// change client offset by amount update scrollbar position
				Vec2f cur = mOrigin;
				ScrollTo(mOrigin + amount);
				return mOrigin - cur;
			}

			//////////////////////////////////////////////////////////////////////

			void ScrollTo(Vec2f const &o)
			{
				Vec2f s(ClientSize() - GetSize());
				Vec2f sp = Max(Vec2f::zero, Min(o, s));
				mScrollTarget = mOrigin = sp;
				mClipRectangle.mClientTransform = TranslationMatrix(Vec4(floorf(-mOrigin.x), floorf(-mOrigin.y), 0));
				UpdateScrollbars();
			}

			//////////////////////////////////////////////////////////////////////

			void SetClientSize(Vec2f const &s)
			{
				mClientSize = s;
				ScrollTo(mOrigin);
			}

			//////////////////////////////////////////////////////////////////////

			void UpdateScrollbars()
			{
				Vec2f s = ClientSize() - GetSize();
				if(s.y > 0)
				{
					float h = Min(Height(), Max(8.0f, Height() / (ClientHeight() / Height())));
					mVerticalScrollBar.SetY((Height() - h) * (mOrigin.y / s.y));
					mVerticalScrollBar.SetHeight(h);
					mVerticalScrollBar.Show();
				}
				else
				{
					mVerticalScrollBar.Hide();
				}
				
				if(s.x > 0)
				{
					float w = Min(Width(), Max(8.0f, Width() / (ClientWidth() / Width())));
					mHorizontalScrollBar.SetX((Width() - w) * (mOrigin.x / s.x));
					mHorizontalScrollBar.SetWidth(w);
					mHorizontalScrollBar.Show();
				}
				else
				{
					mHorizontalScrollBar.Hide();
				}
			}

			//////////////////////////////////////////////////////////////////////

			float ClientHeight() const
			{
				return mClientSize.y;
			}

			//////////////////////////////////////////////////////////////////////

			float ClientWidth() const
			{
				return mClientSize.x;
			}

			//////////////////////////////////////////////////////////////////////

			Vec2f const &ClientSize() const
			{
				return mClientSize;
			}

			//////////////////////////////////////////////////////////////////////

			void SetClientWidth(float w)
			{
				SetClientSize({ w, mClientSize.y });
			}

			//////////////////////////////////////////////////////////////////////

			void SetClientHeight(float h)
			{
				SetClientSize({ mClientSize.x, h });
			}

			//////////////////////////////////////////////////////////////////////

			void SetSize(Vec2f const &size) override
			{
				Element::SetSize(size);
				mClipRectangle.SetSize(size);
				mHorizontalScrollBar.SetPosition({ 0, Height() - mHorizontalScrollBar.Height() });
				mVerticalScrollBar.SetPosition({ Width() - mVerticalScrollBar.Width(), 0 });
				UpdateScrollbars();
			}

			//////////////////////////////////////////////////////////////////////

			virtual void MeasureClientSize()
			{
				Vec2f cs(0, 0);
				for(auto &row : mClipRectangle.mChildren)
				{
					cs = Max(cs, row.GetPosition() + row.GetSize());
				}
				SetClientSize(cs);
			}

			//////////////////////////////////////////////////////////////////////

			virtual void AppendControl(Element &e)
			{
				mClipRectangle.AddChild(e);
				SetClientSize(Max(ClientSize(), e.GetPosition() + e.GetSize()));
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct ListRow: Label
		{
			char const *Name() const override
			{
				return "ListRow";
			}

			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override;

			// only check top and bottom of rectangle (infinite width)
			bool Pick(Message const *m) const override
			{
				if(m->IsMouseMessage())
				{
					MouseMessage const *mm = (MouseMessage const *)m;
					Vec2f const &point = mm->mPosition;
					Vec2f const *r = mScreenCoordinates;
					return UnscaledDistanceToLine(r[1], r[0], point) >= 0 &&
						UnscaledDistanceToLine(r[3], r[2], point) > 0;
				}
				else
				{
					return false;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct ListBox: Window
		{
			Delegate<ControlEvent>		mRowRemoved;
			Delegate<MouseWheelEvent>	mMouseWheelDelegate;

			//////////////////////////////////////////////////////////////////////

			ListBox()
				: Window()
			{
				MouseWheeled += mMouseWheelDelegate = [this] (MouseWheelEvent const &e)
				{
					AddToScrollTarget({0, Height() * -0.15f * e.wheelDelta });
				};

				mClipRectangle.ChildRemoved += mRowRemoved = [this] (ControlEvent const &e)
				{
					MeasureClientSize();
				};;
			} 

			//////////////////////////////////////////////////////////////////////

			char const *Name() const override
			{
				return "ListBox";
			}

			//////////////////////////////////////////////////////////////////////

			void MeasureClientSize() override
			{
				Vec2f cs(0, 0);
				for(auto &row : mClipRectangle.mChildren)
				{
					ListRow &r = (ListRow &)row;
					Vec2f const &pos = row.GetPosition();
					cs.x = Max(cs.x, r.mTextDimensions.x);
					r.SetY(cs.y);
					cs.y = Max(cs.y, cs.y + r.Height());
				}
				SetClientSize(cs);
			}
			
			//////////////////////////////////////////////////////////////////////

			ListRow *AddString(char const *text, Typeface *font)
			{
				ListRow *row = new ListRow();
				row->MouseEntered += [] (MouseEvent const &m)
				{
					m.mElement->Set(eSelected);
				};

				row->MouseLeft += [] (MouseEvent const &m)
				{
					m.mElement->Clear(eSelected);
				};

				row->SetText(text);
				row->SetFont(font);
				row->SetPivot({ 0, 0 });
				row->Set(eTransparent);
				row->SetOffset({ 3, 1 });
				row->SetPosition({ 0, ClientHeight() });
				row->SetHeight(row->GetTextSize().y + 2);
				AppendControl(*row);
				return row;
			}
		};
	}
}
