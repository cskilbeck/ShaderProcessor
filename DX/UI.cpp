#include "DX.h"

#include "Shaders/Image2D.shader.h"
#include "Shaders/Color2D.shader.h"

//////////////////////////////////////////////////////////////////////

namespace
{
	Font::VB fontVB;

	DXShaders::Image2D image2DShader;
	DXShaders::Color2D colorShader;

	VertexBuilder<DXShaders::Image2D::InputVertex> image2DVertBuffer;
	VertexBuilder<DXShaders::Color2D::InputVertex> colorVB;
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
		// You better Execute() the DrawList after this before you draw any more UI with it...

		void Draw(Element *rootElement, ID3D11DeviceContext *context, DrawList &drawList, Matrix const &ortho)
		{
			image2DVertBuffer.Map(context);
			colorVB.Map(context);
			fontVB.Map(context);
			rootElement->Draw(context, drawList, ortho);
			drawList.End();
			fontVB.UnMap(context);
			image2DVertBuffer.UnMap(context);
			colorVB.UnMap(context);
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
				//TRACE("%s::OnDraw\n", Name());
				OnDraw(mTransformMatrix * ortho, context, drawList);
				for(auto &r : mChildren)
				{
					((Element &)r).Draw(context, drawList, ortho);
				}
				OnDrawComplete(drawList);
			}
		}

		//////////////////////////////////////////////////////////////////////

		bool ClipRectangle::IsClipper() const
		{
			return true;
		}

		//////////////////////////////////////////////////////////////////////

		void ClipRectangle::OnDrawComplete(DrawList &drawList)
		{
			Vec4f p[4] = { 0 };
			drawList.SetConstantData(Vertex, p, DXShaders::Color2D::VS::g_ClipPlanes2D_index);
		}

		//////////////////////////////////////////////////////////////////////

		void ClipRectangle::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			// the corners
			Vec4f topLeft = TransformPoint(Vec4(0, 0, 0, 0), matrix);
			Vec4f topRight = TransformPoint(Vec4(mSize.x, 0, 0, 0), matrix);
			Vec4f bottomLeft = TransformPoint(Vec4(0, mSize.y, 0, 0), matrix);

			// the edges
			Vec4f topEdge = topRight - topLeft;
			Vec4f leftEdge = bottomLeft - topLeft;

			// the normals
			Vec4f left = Normalize(Vec4(-GetY(leftEdge), GetX(leftEdge), 0, 0));
			Vec4f top = Normalize(Vec4(GetY(topEdge), -GetX(topEdge), 0, 0));
			Vec4f right = -left;
			Vec4f bottom = -top;

			// the planes
			Vec4f planes[4] =
			{
				SetW(left, Dot(left, topLeft)),
				SetW(right, Dot(right, topRight)),
				SetW(top, Dot(top, topLeft)),
				SetW(bottom, Dot(bottom, bottomLeft)),
			};

			// This is fucked - the only reason to call it is to make the SetConstantData work
			// There must be a better way...
			drawList.SetShader(context, &colorShader, &colorVB);

			// set clip planes into the constant buffer
			drawList.SetConstantData(Vertex, planes, DXShaders::Color2D::VS::g_ClipPlanes2D_index);
		}

		//////////////////////////////////////////////////////////////////////

		void Line::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			drawList.SetShader(context, &colorShader, &colorVB);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Color2D::VS::g_VertConstants2D_index);
			drawList.BeginLineList();
			colorVB.AddVertex({ { 0, 0 }, mColor });
			colorVB.AddVertex({ { mSize.x, mSize.y }, mColor });
			drawList.End();
		}

		//////////////////////////////////////////////////////////////////////

		void OutlineRectangle::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			drawList.SetShader(context, &colorShader, &colorVB);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Color2D::VS::g_VertConstants2D_index);
			drawList.BeginLineStrip();
			colorVB.AddVertex({ { 0, 0 }, mColor });
			colorVB.AddVertex({ { mSize.x, 0 }, mColor });
			colorVB.AddVertex({ { mSize.x, mSize.y }, mColor });
			colorVB.AddVertex({ { 0, mSize.y }, mColor });
			colorVB.AddVertex({ { 0, -1 }, mColor });
			drawList.End();
		}

		//////////////////////////////////////////////////////////////////////

		void FilledRectangle::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			drawList.SetShader(context, &colorShader, &colorVB);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Color2D::VS::g_VertConstants2D_index);
			drawList.BeginTriangleStrip();
			colorVB.AddVertex({ { 0, mSize.y }, mColor });
			colorVB.AddVertex ({ { mSize.x, mSize.y }, mColor });
			colorVB.AddVertex({ { 0, 0 }, mColor });
			colorVB.AddVertex({ { mSize.x, 0 }, mColor });
			drawList.End();
		}

		//////////////////////////////////////////////////////////////////////

		void Label::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			Font f(mTypeface, &drawList, &fontVB);
			f.Start(context, matrix);
			f.DrawString(mText.c_str(), Vec2f(0, 0));
			f.End();
		}

		//////////////////////////////////////////////////////////////////////

		void Image::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			drawList.SetShader(context, &image2DShader, &image2DVertBuffer);
			drawList.SetTexture(Pixel, *mGraphic);
			drawList.SetSampler(Pixel, *mSampler);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Image2D::VS::g_VertConstants2D_index);
			drawList.BeginTriangleStrip();
			image2DVertBuffer.AddVertex({ { 0, mSize.y }, { 0, 1 }, 0xffffffff });
			image2DVertBuffer.AddVertex({ { mSize.x, mSize.y }, { 1, 1 }, 0xffffffff });
			image2DVertBuffer.AddVertex({ { 0, 0}, { 0, 0 }, 0xffffffff });
			image2DVertBuffer.AddVertex({ { mSize.x, 0}, { 1, 0 }, 0xffffffff });
			drawList.End();
		}
	}
}