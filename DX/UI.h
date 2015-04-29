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

		struct MouseClickedEvent : UIEvent
		{
			Vec2f	mMousePosition;
		};

		struct Element : Aligned16, list_node<Element>
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
			linked_list<Element>		mChildren;
			Element *					mParent;
			float						mTransparency;
			int							mZIndex;
			Flags32						mFlags;
		private:
			Vec2f						mPosition;
			Vec2f						mSize;
			Vec2f						mPivot;

			template<uint32 f> bool Is() const
			{
				return mFlags(f);
			}

			template<uint32 f> void Set(bool s)
			{
				if(s)
				{
					mFlags.Set(f);
				}
				else
				{
					mFlags.Clear(f);
				}
			}

			bool IsVisible() const
			{
				return Is<eVisible>();
			}

			void SetVisible(bool s)
			{
				Set<eVisible>(s);
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
				Set<eDirty>(true);
			}

			Vec2f const &GetSize() const
			{
				return mSize;
			}

			void SetSize(Vec2f size)
			{
				mSize = size;
				Set<eDirty>(true);
			}

			Vec2f const &GetPivot() const
			{
				return mPivot;
			}

			void SetPivot(Vec2f pivot)
			{
				mPivot = pivot;
				Set<eDirty>(true);
			}

			void UpdateMatrix()
			{
				// calculate matrix based on position, size & pivot
			}

			bool						mActive;
			bool						mEnabled;
			bool						mClosed;
			bool						mModal;

			Event<MouseClickedEvent>	OnMouseClicked;

			void AddChild(Element &e)
			{
				mChildren.push_back(e);
				e.mParent = this;
			}

			void RemoveChild(Element &e)
			{
				mChildren.remove(e);
				e.mParent = null;
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
				UpdateMatrix();
				Matrix m = transform * mTransform;
				// stash matrix into Shader here
				OnDraw();
				for(auto &r : mChildren)
				{
					r.Draw(window, m);
				}
			}

			void Update(float deltaTime)
			{
				OnUpdate(deltaTime);
				for(auto &r : mChildren)
				{
					r.Update(deltaTime);
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
