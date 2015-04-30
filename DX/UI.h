#pragma once

namespace DX
{
	namespace UI
	{
		struct Element;

		struct UIEvent
		{
			Element *mElement;
		};

		struct ClickedEvent : UIEvent
		{
			Vec2f	mMousePosition;
		};

		struct ElementBase: Aligned16, list_node<ElementBase>
		{
			ElementBase *				mParent;
		};

		struct Element : ElementBase
		{
			enum: uint32
			{
				eVisible = 1,		// draw it or don't
				eActive = 2,		// call OnUpdate() or don't
				eEnabled = 4,		// process input messages or don't
				eClosed = 8,		// close requested, will happen at the end of the frame
				eModal = 16			// block parental input
			};

			Matrix						mTransform;	// don't mess with this
			linked_list<ElementBase>	mChildren;
			int							mZIndex;
			Flags32						mFlags;
			Vec2f						mPosition;
			Vec2f						mSize;
			Vec2f						mScale;
			Vec2f						mPivot;
			Matrix						mMatrix;

			Event<ClickedEvent>	OnClicked;

			void AddChild(Element &e)
			{
				mChildren.push_back((ElementBase &)e);
				e.mParent = this;
			}

			void RemoveChild(Element &e)
			{
				mChildren.remove((ElementBase &)e);
				e.mParent = null;
			}

			bool Is(uint32 f) const
			{
				return mFlags.IsAnySet(f);
			}

			void Set(uint32 f)
			{
				mFlags.Set(f);
			}

			void Clear(uint32 f)
			{
				mFlags.Clear(f);
			}

			void SetFlag(uint32 f, bool v = true)
			{
				if(v)
				{
					Set(f);
				}
				else
				{
					Clear(f);
				}
			}

			bool IsVisible() const
			{
				return Is(eVisible);
			}

			void SetVisible(bool s)
			{
				Set(eVisible);
			}

			void Show()
			{
				SetVisible(true);
			}

			void Hide()
			{
				SetVisible(false);
			}

			Vec2f GetPosition() const
			{
				return mPosition;
			}

			void SetPosition(Vec2f pos)
			{
				mPosition = pos;
			}

			Vec2f GetSize() const
			{
				return mSize;
			}

			void SetSize(Vec2f size)
			{
				mSize = size;
			}

			Vec2f GetPivot() const
			{
				return mPivot;
			}

			void SetPivot(Vec2f pivot)
			{
				mPivot = pivot;
			}

			virtual void OnDraw()
			{
			}

			virtual void OnHover()
			{
			}

			virtual void OnUpdate(float deltaTime)
			{
			}

			void Draw(DXWindow *window, Matrix &transform)
			{
				Matrix m = transform * mTransform;
				OnDraw();
				for(auto &r : mChildren)
				{
					Element &e = (Element &)r;
					e.Draw(window, m);
				}
			}

			void Update(float deltaTime)
			{
				OnUpdate(deltaTime);
				for(auto &r : mChildren)
				{
					Element &e = (Element &)r;
					e.Update(deltaTime);
				}
			}
		};

		struct Label : Element
		{
			tstring						mText;
			Font *						mFont;
		};

		struct Button : Label
		{
			Texture *					mGraphic;
		};
	}
}
