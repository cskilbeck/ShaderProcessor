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

		void ClipRectangle::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			Vec4f corners[4] =
			{
				Vec4(0, 0, 0, 0),				// topleft
				Vec4(mSize.x, 0, 0, 0),			// topright
				Vec4(0, mSize.y, 0, 0),			// bottomleft
				Vec4(mSize.x, mSize.y, 0, 0)	// bottomright
			};

			for(int i = 0; i < 4; ++i)
			{
				corners[i] = TransformPoint(corners[i], matrix);
			}

			// now work out the normals
			Vec4f p[4] =
			{
				Normalize(corners[1] - corners[0]),
				Normalize(corners[0] - corners[1]),
				Normalize(corners[2] - corners[0]),
				Normalize(corners[0] - corners[2])
			};

			// and the planes
			for(int i = 0; i < 4; ++i)
			{
				p[i] = SetW(p[i], Dot(p[i], corners[i]));
			}

			// set clip planes into the constant buffer
			drawList.SetConstantData(Vertex, p, DXShaders::Color2D::VS::g_ClipPlanes2D_index);
		}

		//////////////////////////////////////////////////////////////////////

		void Rectangle::OnDraw(Matrix const &matrix, ID3D11DeviceContext *context, DrawList &drawList)
		{
			static int t = 0;
			t += 1;
			float f = t / PI / 8;
			float x = sinf(f);
			float y = cosf(f);
			float ar = 720.0f / 1280.0f;
			Vec4f v[4] =
			{
				Vec4(-y, x * ar, 0, -0.25f),
				Vec4(y, -x * ar, 0, -0.25f),
				Vec4(x, y * ar, 0, -0.25f),
				Vec4(-x, -y * ar, 0, -0.25f)
			};

			drawList.Reset(context, &colorShader, &colorVB);
			drawList.SetConstantData(Vertex, Transpose(matrix), DXShaders::Color2D::VS::g_VertConstants2D_index);
			drawList.SetConstantData(Vertex, v, DXShaders::Color2D::VS::g_ClipPlanes2D_index);
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