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
			colorShader.Create();
			image2DVertBuffer.Create(8192);
			fontVB.Create(8192);
			colorVB.Create(8192);
		}

		//////////////////////////////////////////////////////////////////////

		void Close()
		{
			image2DShader.Release();
			colorShader.Release();
			image2DVertBuffer.Destroy();
			fontVB.Destroy();
			colorVB.Destroy();
		}

		//////////////////////////////////////////////////////////////////////

		void Draw(Element *rootElement, ID3D11DeviceContext *context, DrawList &drawList, Matrix const &ortho)
		{
			rootElement->Draw(context, drawList, ortho);
			Vec4f p[4] = { 0 };
			drawList.SetConstantData(Vertex, p, DXShaders::Color2D::VS::g_ClipPlanes2D_index);
		}

		//////////////////////////////////////////////////////////////////////

		void Update(Element *rootElement, float deltaTime)
		{
			rootElement->Update(deltaTime, IdentityMatrix, false);
		}

		//////////////////////////////////////////////////////////////////////

		void Element::Draw(ID3D11DeviceContext *context, DrawList &drawList, Matrix const &ortho)
		{
			if(IsVisible())
			{
				SortChildren();
				OnDraw(mTransformMatrix * ortho, context, drawList);
				for(auto &r : mChildren)
				{
					((Element &)r).Draw(context, drawList, ortho);
				}
			}
		}

		//////////////////////////////////////////////////////////////////////

		bool ClipRectangle::IsClipper() const
		{
			return true;
		}

		//////////////////////////////////////////////////////////////////////

		void ClipRectangle::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			// the clip rectangle corners
			Vec4f topLeft = TransformPoint(Vec4(0, 0, 0, 0), matrix);
			Vec4f topRight = TransformPoint(Vec4(mSize.x, 0, 0, 0), matrix);
			Vec4f bottomLeft = TransformPoint(Vec4(0, mSize.y, 0, 0), matrix);
			Vec4f bottomRight = TransformPoint(Vec4(mSize.x, mSize.y, 0, 0), matrix);

			// get the edges
			Vec4f topEdge = topRight - topLeft;
			Vec4f leftEdge = bottomLeft - topLeft;
			Vec4f bottomEdge = bottomRight - bottomLeft;
			Vec4f rightEdge = bottomRight - topRight;

			// now work out the normals
			Vec4f left = Normalize(Vec4(-GetY(leftEdge), GetX(leftEdge), 0, 0));
			Vec4f right = Normalize(Vec4(GetY(rightEdge), -GetX(rightEdge), 0, 0));
			Vec4f top = Normalize(Vec4(GetY(topEdge), -GetX(topEdge), 0, 0));
			Vec4f bottom = Normalize(Vec4(-GetY(bottomEdge), GetX(bottomEdge), 0, 0));

			// now the planes
			Vec4f planes[4] =
			{
				SetW(left, Dot(left, topLeft)),
				SetW(right, Dot(right, topRight)),
				SetW(top, Dot(top, topLeft)),
				SetW(bottom, Dot(bottom, bottomLeft)),
			};

			// set clip planes into the constant buffer
			drawList.SetConstantData(Vertex, planes, DXShaders::Color2D::VS::g_ClipPlanes2D_index);
		}

		//////////////////////////////////////////////////////////////////////

		void Rectangle::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			drawList.Reset(context, &colorShader, &colorVB);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Color2D::VS::g_VertConstants2D_index);
			drawList.BeginTriangleStrip();
			using Vert = DXShaders::Color2D::InputVertex;
			drawList.AddVertex<Vert>({ { 0, mSize.y }, mColor });
			drawList.AddVertex<Vert>({ { mSize.x, mSize.y }, mColor });
			drawList.AddVertex<Vert>({ { 0, 0 }, mColor });
			drawList.AddVertex<Vert>({ { mSize.x, 0 }, mColor });
			drawList.End();
		}

		//////////////////////////////////////////////////////////////////////

		void Label::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			mFont.Init(mTypeface, &drawList, &fontVB);
			mFont.Begin(context, matrix);
			mFont.DrawString(mText.c_str(), Vec2f(0, 0));
			mFont.End();
		}

		//////////////////////////////////////////////////////////////////////

		void Image::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			drawList.Reset(context, &image2DShader, &image2DVertBuffer);
			drawList.SetTexture(Pixel, *mGraphic);
			drawList.SetSampler(Pixel, *mSampler);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Image2D::VS::g_VertConstants2D_index);
			drawList.BeginTriangleStrip();
			drawList.AddVertex<Vert>({ { 0, mSize.y }, { 0, 1 }, 0xffffffff });
			drawList.AddVertex<Vert>({ { mSize.x, mSize.y }, { 1, 1 }, 0xffffffff });
			drawList.AddVertex<Vert>({ { 0, 0}, { 0, 0 }, 0xffffffff });
			drawList.AddVertex<Vert>({ { mSize.x, 0}, { 1, 0 }, 0xffffffff });
			drawList.End();
		}
	}
}