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

		struct ElementBase: list_node<ElementBase>
		{
			ElementBase *				mParent;
		};

		struct Element : Aligned16, ElementBase
		{
			enum: uint32
			{
				eVisible = 1,
				eActive = 2,
				eEnabled = 4,
				eClosed = 8,
				eModel = 16,
				eDirty = 32
			};

			Matrix						mTransform;	// don't mess with this
			linked_list<ElementBase>	mChildren;
			float						mTransparency;
			int							mZIndex;
			Flags32						mFlags;
		private:
			Vec2f						mPosition;
			Vec2f						mSize;
			Vec2f						mPivot;

			void AddChild(ElementBase &e)
			{
				mChildren.push_back(e);
				e.mParent = this;
			}

			void RemoveChild(ElementBase &e)
			{
				mChildren.remove(e);
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

			void SetFlag(uint32 f, bool v)
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

			Vec2f const &GetPosition() const
			{
				return mPosition;
			}

			void SetPosition(Vec2f pos)
			{
				mPosition = pos;
				Set(eDirty);
			}

			Vec2f const &GetSize() const
			{
				return mSize;
			}

			void SetSize(Vec2f size)
			{
				mSize = size;
				Set(eDirty);
			}

			Vec2f const &GetPivot() const
			{
				return mPivot;
			}

			void SetPivot(Vec2f pivot)
			{
				mPivot = pivot;
				Set(eDirty);
			}

			void UpdateMatrix()
			{
				// calculate matrix based on position, size & pivot
				Clear(eDirty);
			}

			bool						mActive;
			bool						mEnabled;
			bool						mClosed;
			bool						mModal;

			Event<ClickedEvent>	OnClicked;

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
				UpdateMatrix();
				Matrix m = transform * mTransform;
				// stash matrix into Shader here
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
