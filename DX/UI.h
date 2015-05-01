//////////////////////////////////////////////////////////////////////
// TODO (charlie): fix how janky and lame this is

#pragma once

#include "Shaders/Image2D.shader.h"

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	namespace UI
	{
		//////////////////////////////////////////////////////////////////////

		struct Element;

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
			ElementBase *mParent;
			int mZIndex;

			bool operator < (ElementBase &o)
			{
				return mZIndex < o.mZIndex;
			}

			ElementBase()
				: mParent(null)
				, mZIndex(0)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct Element: ElementBase
		{
			enum: uint32
			{
				eVisible = 1,		// draw it or don't
				eActive = 2,		// call OnUpdate() or don't
				eEnabled = 4,		// process input messages or don't
				eClosed = 8,		// close requested, will happen at the end of the frame
				eModal = 16,		// block parental input
				eReorder = 32		// child was added, sort children
			};

			linked_list<ElementBase>	mChildren;
			Flags32						mFlags;
			Vec2f						mPosition;
			Vec2f						mSize;
			Vec2f						mScale;
			Vec2f						mPivot;
			float						mAngle;

			Event<ClickedEvent>	Clicked;
			Event<PressedEvent> Pressed;
			Event<ReleasedEvent> Released;
			Event<UIEvent> Updated;
			Event<MouseEvent> MouseEntered;
			Event<MouseEvent> MouseLeft;

			//////////////////////////////////////////////////////////////////////

			Element()
				: ElementBase()
				, mFlags(0)
				, mPosition(0, 0)
				, mSize(0, 0)
				, mScale(1, 1)
				, mPivot(0, 0)
				, mAngle(0)
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

			bool Is(uint32 f) const
			{
				return mFlags.IsAnySet(f);
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
				if(v)
				{
					Set(f);
				}
				else
				{
					Clear(f);
				}
			}

			//////////////////////////////////////////////////////////////////////

			bool IsVisible() const
			{
				return Is(eVisible);
			}

			//////////////////////////////////////////////////////////////////////

			Element &SetVisible(bool s)
			{
				Set(eVisible);
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
				return *this;
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
				return *this;
			}

			//////////////////////////////////////////////////////////////////////

			virtual void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
			{
			}

			//////////////////////////////////////////////////////////////////////

			virtual void OnHover()
			{
			}

			//////////////////////////////////////////////////////////////////////

			virtual void OnUpdate(float deltaTime)
			{
				Updated.Invoke(this);
			}

			//////////////////////////////////////////////////////////////////////

			void Draw(ID3D11DeviceContext *context, DrawList &drawList, Matrix &transform)
			{
				if(Is(eReorder))
				{
					mChildren.sort();
				}
				Vec2f p = mPivot * -mSize;
				Matrix tr =
					TranslationMatrix(Vec4(p.x, p.y, 0)) *						// place it around the pivot point
					ScaleMatrix(Vec4(mScale.x, mScale.y, 1)) *				// scale it
					RotationMatrix(0, 0, mAngle) *							// rotate it
					TranslationMatrix(Vec4(mPosition.x, mPosition.y, 0)) *	// translate it
					transform;
				OnDraw(tr, context, drawList);
				for(auto &r : mChildren)
				{
					((Element &)r).Draw(context, drawList, tr);
				}
			}

			//////////////////////////////////////////////////////////////////////

			void Update(float deltaTime)
			{
				OnUpdate(deltaTime);
				for(auto &r : mChildren)
				{
					((Element &)r).Update(deltaTime);
				}
			}
		};

		//////////////////////////////////////////////////////////////////////

		struct Label: Element
		{
			string mText;
			Font *mFont;

			//////////////////////////////////////////////////////////////////////

			Label()
				: Element()
				, mFont(null)
			{
			}

			//////////////////////////////////////////////////////////////////////

			void Measure()
			{
				if(!mText.empty() && mFont != null)
				{
					mSize = mFont->MeasureString(mText.c_str());
					float hb = mFont->GetBaseline() / mFont->GetHeight() / 2;
					SetPivot(Vec2f(0.5f, hb));
				}
			}

			//////////////////////////////////////////////////////////////////////

			Label &SetFont(Font *font)
			{
				mFont = font;
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

			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override
			{
				mFont->SetDrawList(drawList);
				mFont->Setup(context, matrix);
				mFont->Begin();
				mFont->DrawString(mText.c_str(), Vec2f(0, 0), Font::HLeft, Font::VTop);
				mFont->End();
			}
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

			void OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList) override
			{
				using namespace DXShaders;
				using Vert = Image2D::InputVertex;

				static Image2D image2DShader;
				static Image2D::VertBuffer image2DVertBuffer;

				if(!image2DShader.IsInitialized())
				{
					image2DShader.Create();
					image2DVertBuffer.Create(16384);
				}

				drawList.Reset(context, &image2DShader, &image2DVertBuffer);
				drawList.SetTexture(Pixel, *mGraphic);
				drawList.SetSampler(Pixel, *mSampler);
				drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Image2D::VS::vConstants_index);
				drawList.BeginTriangleStrip();
				drawList.AddVertex<Vert>({ { 0000000, mSize.y }, { 0, 1 }, 0xffffffff });
				drawList.AddVertex<Vert>({ { mSize.x, mSize.y }, { 1, 1 }, 0xffffffff });
				drawList.AddVertex<Vert>({ { 0000000, 0000000 }, { 0, 0 }, 0xffffffff });
				drawList.AddVertex<Vert>({ { mSize.x, 0000000 }, { 1, 0 }, 0xffffffff });
				drawList.End();
			}
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

			LabelButton &SetFont(Font *f)
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
