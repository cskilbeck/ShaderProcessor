#include "DX.h"

#include "Shaders/Image2D.shader.h"
#include "Shaders/Color2D.shader.h"

//////////////////////////////////////////////////////////////////////

namespace
{
	using UIShader = DXShaders::Image2D;
	using Vert = UIShader::InputVertex;
	Font::VB fontVB;
	UIShader image2DShader;
	UIShader::VertBuffer image2DVertBuffer;
	DXShaders::Color2D colorShader;
	DXShaders::Color2D::VertBuffer colorVB;
}

namespace DX
{
	namespace UI
	{
		//////////////////////////////////////////////////////////////////////

		void Open()
		{
			image2DShader.Create();
			image2DVertBuffer.Create(8192);
			colorShader.Create();
			fontVB.Create(8192);
			colorVB.Create(8192);
		}

		//////////////////////////////////////////////////////////////////////

		void Close()
		{
			image2DShader.Release();
			image2DVertBuffer.Release();
			colorShader.Release();
			fontVB.Release();
			colorVB.Release();
		}

		//////////////////////////////////////////////////////////////////////

		void Element::Draw(ID3D11DeviceContext *context, DrawList &drawList, Matrix &transform)
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

		void Rectangle::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			drawList.Reset(context, &colorShader, &colorVB);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Color2D::VS::VertConstants_index);
			drawList.BeginTriangleStrip();
			using Vert = DXShaders::Color2D::InputVertex;
			drawList.AddVertex<Vert>({ { 0, mSize.y }, mColor });
			drawList.AddVertex<Vert>({ { mSize.x, mSize.y }, mColor });
			drawList.AddVertex<Vert>({ { 0, 0 }, mColor });
			drawList.AddVertex<Vert>({ { mSize.x, 0 }, mColor });
			drawList.End();
		}

		void Label::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			mFontInstance.Init(mFont, &drawList, &fontVB);
			mFontInstance.Begin(context, matrix);
			mFontInstance.DrawString(mText.c_str(), Vec2f(0, 0));
			mFontInstance.End();
		}

		void Image::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
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
	}
}