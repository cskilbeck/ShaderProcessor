//////////////////////////////////////////////////////////////////////

#include "DX.h"
#include "Shaders/Debug.shader.h"
#include "Shaders/Debug2D.shader.h"
#include "Shaders/Font.shader.h"

//////////////////////////////////////////////////////////////////////

using namespace DX;

namespace
{
	enum primType
	{
		None = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED,
		Lines = D3D10_PRIMITIVE_TOPOLOGY_LINELIST,
		Triangles = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	};

	template <typename T, typename V> struct DebugContext
	{
		using VB = VertexBuilder<typename T::InputVertex>;

		T			mShader;
		DrawList	mDrawList;
		VB			mVertexBuilder;
		primType	mPrimType;

		HRESULT Create()
		{
			DXR(mShader.Create());
			DXR(mVertexBuilder.Create(32768));
			return S_OK;
		}

		void Destroy()
		{
			mShader.Release();
			mVertexBuilder.Destroy();
		}

		void Begin(ID3D11DeviceContext *context)
		{
			mVertexBuilder.Map(context);
			mDrawList.SetShader(context, &mShader, &mVertexBuilder);
			mPrimType = None;
		}

		void End(ID3D11DeviceContext *context)
		{
			mDrawList.End();
			mVertexBuilder.UnMap(context);
			mDrawList.Execute();
		}

		void SetTransform(Matrix const &m, uint index)
		{
			mDrawList.SetConstantData(Vertex, m, index);
		}

		void AddLine(V const &start, V const &end, Color color)
		{
			SetPrimType(Lines);
			mVertexBuilder.AddVertex({ start, color });
			mVertexBuilder.AddVertex({ end, color });
		}

		void AddTriangle(V const &a, V const &b, V const &c, Color color)
		{
			SetPrimType(Triangles);
			mVertexBuilder.AddVertex({ a, color });
			mVertexBuilder.AddVertex({ b, color });
			mVertexBuilder.AddVertex({ c, color });
		}

		void AddQuad(V const &a, V const &b, V const &c, V const &d, Color color)
		{
			SetPrimType(Triangles);
			mVertexBuilder.AddVertex({ a, color });
			mVertexBuilder.AddVertex({ b, color });
			mVertexBuilder.AddVertex({ c, color });
			mVertexBuilder.AddVertex({ c, color });
			mVertexBuilder.AddVertex({ d, color });
			mVertexBuilder.AddVertex({ a, color });
		}

		void SetPrimType(primType primType)
		{
			if(primType != mPrimType)
			{
				mDrawList.End();
				mDrawList.Begin(primType);
				mPrimType = primType;
			}
		}
	};

	DebugContext<DXShaders::Debug, Vec4f> debug3D;
	DebugContext<DXShaders::Debug2D, Vec2f> debug2D;

	DXPtr<Typeface> debugFont;
	DrawList debugTextDrawList;
	Font fontInstance;
	VertexBuilder<DXShaders::Font::InputVertex> fontVB;
	DXWindow *mainWindow;
	Vec4f cameraPos;
	Vec2f cursorPos;
	primType currentPrimType;
}

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	HRESULT debug_open(DXWindow *w)
	{
		assert(FontManager::IsOpen());
		DXR(FontManager::Load("debug", &debugFont));
		DXR(fontVB.Create(8192));
		debug3D.Create();
		debug2D.Create();
		fontInstance.Init(debugFont, &debugTextDrawList, &fontVB);
		mainWindow = w;
		currentPrimType = None;
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////

	void debug_close()
	{
		debug3D.Destroy();
		debug2D.Destroy();
		fontVB.Destroy();
		debugFont.Release();
		mainWindow = null;
	}

	//////////////////////////////////////////////////////////////////////

	void debug_begin()
	{
		cursorPos = Vec2f::zero;
		fontInstance.Begin(mainWindow->Context(), mainWindow);
		debug3D.Begin(mainWindow->Context());
		debug2D.Begin(mainWindow->Context());
		debug2D.SetTransform(Transpose(OrthoProjection2D(mainWindow->ClientWidth(), mainWindow->ClientHeight())), DXShaders::Debug2D::VS::VertConstants_index);
	}

	//////////////////////////////////////////////////////////////////////

	void debug_set_camera(Camera const &camera)
	{
		debug3D.SetTransform(Transpose(camera.GetTransformMatrix()), DXShaders::Debug::VS::VertConstants_index);
		cameraPos = camera.GetPosition();
	}

	//////////////////////////////////////////////////////////////////////

	void debug_end()
	{
		debug2D.End(mainWindow->Context());
		debug3D.End(mainWindow->Context());
		debugTextDrawList.End();
		fontInstance.Finished(mainWindow->Context());
		mainWindow->ResetRenderTargetView();
		debugTextDrawList.Execute();
	}

	//////////////////////////////////////////////////////////////////////

	void debug_text(char const *fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		string s = Format_V(fmt, v);
		fontInstance.DrawString(s.c_str(), cursorPos, &cursorPos);
	}

	//////////////////////////////////////////////////////////////////////

	void debug_text(wchar const *fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		string s = StringFromWideString(Format_V(fmt, v));
		fontInstance.DrawString(s.c_str(), cursorPos, &cursorPos);
	}

	//////////////////////////////////////////////////////////////////////

	void debug_text(int x, int y, char const *fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		string s = Format_V(fmt, v);
		fontInstance.DrawString(s.c_str(), Vec2f((float)x, (float)y));
	}

	//////////////////////////////////////////////////////////////////////

	void debug_text(int x, int y, wchar const *fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		string s = StringFromWideString(Format_V(fmt, v));
		fontInstance.DrawString(s.c_str(), Vec2f((float)x, (float)y));
	}

	//////////////////////////////////////////////////////////////////////

	void debug_line(CVec4f start, CVec4f end, Color color)
	{
		debug3D.AddLine(start, end, color);
	}

	//////////////////////////////////////////////////////////////////////

	void debug_triangle(CVec4f a, CVec4f b, CVec4f c, Color color)
	{
		debug3D.AddTriangle(a, b, c, color);
	}

	//////////////////////////////////////////////////////////////////////

	void debug_quad(CVec4f a, CVec4f b, CVec4f c, CVec4f d, Color color)
	{
		debug3D.AddQuad(a, b, c, d, color);
	}

	//////////////////////////////////////////////////////////////////////
	// a screen facing quad of 2x2 pixels

	void debug_dot(CVec4f pos, Color color)
	{
		Vec4f d = pos - cameraPos;
		float l = Length(d) / 640;	// screenwidth / 2
		Vec4f s = Normalize(Cross(d, Vec4(0, 0, 1))) * l;
		Vec4f u = Normalize(Cross(d, s)) * l;
		debug3D.AddQuad(pos - s - u, pos + s - u, pos + s + u, pos - s + u, color);
	}

	//////////////////////////////////////////////////////////////////////

	void debug_axes(CVec4f pos, float len)
	{
		Vec4f Z = Vec4(0, 0, len);
		Vec4f Y = Vec4(0, len, 0);
		Vec4f X = Vec4(len, 0, 0);
		debug3D.AddLine(pos - Z, pos + Z, Color::BrightBlue);
		debug3D.AddLine(pos - Y, pos + Y, Color::BrightGreen);
		debug3D.AddLine(pos - Z, pos + Z, Color::BrightRed);
	}

	//////////////////////////////////////////////////////////////////////

	void debug_cube(CVec4f bottomLeft, CVec4f topRight, Color color)
	{
		Vec4f d = topRight - bottomLeft;
		Vec4f x = Vec4(GetX(d), 0, 0);
		Vec4f y = Vec4(0, GetY(d), 0);
		Vec4f z = Vec4(0, 0, GetZ(d));

		// bottom square
		debug3D.AddLine(bottomLeft, bottomLeft + x, color);
		debug3D.AddLine(bottomLeft, bottomLeft + y, color);
		debug3D.AddLine(bottomLeft + x, bottomLeft + x + y, color);
		debug3D.AddLine(bottomLeft + y, bottomLeft + x + y, color);

		// verticals
		debug3D.AddLine(bottomLeft, bottomLeft + z, color);
		debug3D.AddLine(bottomLeft + x, bottomLeft + x + z, color);
		debug3D.AddLine(bottomLeft + y, bottomLeft + y + z, color);
		debug3D.AddLine(bottomLeft + x + y, topRight, color);

		// top square
		Vec4f bl = bottomLeft + z;
		debug3D.AddLine(bl, bl + x, color);
		debug3D.AddLine(bl, bl + y, color);
		debug3D.AddLine(bl + x, bl + x + y, color);
		debug3D.AddLine(bl + y, topRight, color);
	}

	//////////////////////////////////////////////////////////////////////

	void debug_cylinder(Matrix const &matrix, Color color)
	{
		int steps = 16;
		Vec4f oe;
		Vec4f ob;
		for(int i = 0; i <= steps; ++i)
		{
			float t = i * PI * 2 / steps;
			float s, c;
			DirectX::XMScalarSinCos(&s, &c, t);
			Vec4f e = TransformPoint(Vec4(s, c, 1), matrix);
			Vec4f b = TransformPoint(Vec4(s, c, -1), matrix);
			if(i < steps)
			{
				debug3D.AddLine(e, b, color);
			}
			if(i > 0)
			{
				debug3D.AddLine(oe, e, color);
				debug3D.AddLine(ob, b, color);
			}
			oe = e;
			ob = b;
		}
	}

	//////////////////////////////////////////////////////////////////////

	void debug_cylinder(CVec4f start, CVec4f end, float radius, Color color)
	{
		Matrix rotationMatrix;
		Vec4f d(end - start);
		if(fabsf(GetX(d)) > FLT_EPSILON || fabsf(GetY(d)) > FLT_EPSILON)
		{
			Vec4f R2 = Normalize(d);
			Vec4f R0 = Normalize(Cross(Vec4(0, 0, 1), R2));
			Vec4f R1 = Cross(R2, R0);
			rotationMatrix.r[0] = R0;
			rotationMatrix.r[1] = R1;
			rotationMatrix.r[2] = R2;
			rotationMatrix.r[3] = Vec4(0, 0, 0, 1);
		}
		else
		{
			rotationMatrix = IdentityMatrix;
		}
		debug_cylinder(
			TranslationMatrix(Vec4(0, 0, 1)) *
			ScaleMatrix(Vec4(radius, radius, Length(d) / 2)) *
			rotationMatrix *
			TranslationMatrix(start)
			, color);
	}

	void debug_line2d(Vec2f const &a, Vec2f const &b, Color color)
	{
		debug2D.AddLine(a, b, color);
	}

	void debug_solid_rect2d(Vec2f const &tl, Vec2f const &br, Color color)
	{
		debug2D.AddQuad(tl, { br.x, tl.y }, br, { tl.x, br.y }, color);
	}

	void debug_solid_quad2d(Vec2f const *p, Color color)
	{
		debug2D.AddQuad(p[0], p[1], p[2], p[3], color);
	}

	void debug_outline_quad2d(Vec2f const *p, Color color)
	{
		debug_line2d(p[0], p[1], color);
		debug_line2d(p[1], p[2], color);
		debug_line2d(p[2], p[3], color);
		debug_line2d(p[3], p[0], color);
	}

	void debug_outline_rect2d(Vec2f const &tl, Vec2f const &br, Color color)
	{
		Vec2f tr = { br.x, tl.y };
		Vec2f bl = { tl.x, br.y };
		debug_line2d(tl, tr, color);
		debug_line2d(tr, br, color);
		debug_line2d(br, bl, color);
		debug_line2d(bl, tl, color);
	}

	void debug_outline_solid_rect2d(Vec2f const &tl, Vec2f const &br, Color fillColor, Color lineColor)
	{
		debug_solid_rect2d(tl, br, fillColor);
		debug_outline_rect2d(tl, br, lineColor);
	}
}
