//////////////////////////////////////////////////////////////////////

#include "DX.h"
#include "Shaders/Debug.shader.h"
#include "Shaders/Font.shader.h"

//////////////////////////////////////////////////////////////////////

using namespace DX;

namespace
{
	DXPtr<Font> debugFont;
	DXShaders::Debug lineShader;
	DXShaders::Debug::VertBuffer lineVB;
	DrawList debugDrawList;
	DrawList debugLineList;
	DXWindow *mainWindow;
	Vec2f cursorPos;
}

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	HRESULT debug_open(DXWindow *w)
	{
		assert(FontManager::IsOpen());
		DXR(FontManager::Load("debug", &debugFont));

		DXR(lineShader.Create());
		DXR(lineVB.Create(32768));
		mainWindow = w;
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////

	void debug_close()
	{
		debugFont.Release();
		mainWindow = null;
	}

	//////////////////////////////////////////////////////////////////////

	void debug_begin(Camera &camera)
	{
		cursorPos = Vec2f::zero;

		debugFont->SetDrawList(debugDrawList);
		debugFont->Setup(mainWindow->Context(), mainWindow);
		debugFont->Begin();

		debugLineList.Reset(mainWindow->Context(), &lineShader, &lineVB);
		debugLineList.SetConstantData(Vertex, Transpose(camera.GetTransformMatrix()), DXShaders::Debug::VS::VertConstants_index);
		debugLineList.BeginLineList();
	}

	//////////////////////////////////////////////////////////////////////

	void debug_end()
	{
		debugLineList.End();
		debugFont->End();
		debugDrawList.End();
		mainWindow->ResetRenderTargetView();
		debugDrawList.Execute();
		debugLineList.Execute();
	}

	//////////////////////////////////////////////////////////////////////

	void debug_text(char const *fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		string s = Format_V(fmt, v);
		debugFont->DrawString(s.c_str(), cursorPos);
	}

	//////////////////////////////////////////////////////////////////////

	void debug_text(wchar const *fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		string s = StringFromWideString(Format_V(fmt, v));
		debugFont->DrawString(s.c_str(), cursorPos);
	}

	//////////////////////////////////////////////////////////////////////

	void debug_text(int x, int y, char const *fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		string s = Format_V(fmt, v);
		debugFont->DrawString(s.c_str(), Vec2f((float)x, (float)y));
	}

	//////////////////////////////////////////////////////////////////////

	void debug_text(int x, int y, wchar const *fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		string s = StringFromWideString(Format_V(fmt, v));
		debugFont->DrawString(s.c_str(), Vec2f((float)x, (float)y));
	}

	//////////////////////////////////////////////////////////////////////

	void debug_line(Vec4f start, Vec4f end, Color color)
	{
		debugLineList.AddVertex<DXShaders::Debug::InputVertex>({ start, color });
		debugLineList.AddVertex<DXShaders::Debug::InputVertex>({ end, color });
	}

	//////////////////////////////////////////////////////////////////////

	void debug_cube(Vec4f bottomLeft, Vec4f topRight, Color color)
	{
		Vec4f d = topRight - bottomLeft;
		Vec4f x = Vec4(GetX(d), 0, 0);
		Vec4f y = Vec4(0, GetY(d), 0);
		Vec4f z = Vec4(0, 0, GetZ(d));

		// bottom square
		debug_line(bottomLeft, bottomLeft + x, color);
		debug_line(bottomLeft, bottomLeft + y, color);
		debug_line(bottomLeft + x, bottomLeft + x + y, color);
		debug_line(bottomLeft + y, bottomLeft + x + y, color);

		// verticals
		debug_line(bottomLeft, bottomLeft + z, color);
		debug_line(bottomLeft + x, bottomLeft + x + z, color);
		debug_line(bottomLeft + y, bottomLeft + y + z, color);
		debug_line(bottomLeft + x + y, topRight, color);

		// top square
		bottomLeft += z;
		debug_line(bottomLeft, bottomLeft + x, color);
		debug_line(bottomLeft, bottomLeft + y, color);
		debug_line(bottomLeft + x, bottomLeft + x + y, color);
		debug_line(bottomLeft + y, topRight, color);
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
				debug_line(e, b, color);
			}
			if(i > 0)
			{
				debug_line(oe, e, color);
				debug_line(ob, b, color);
			}
			oe = e;
			ob = b;
		}
	}

	//////////////////////////////////////////////////////////////////////

	void debug_cylinder(Vec4f start, Vec4f end, float radius, Color color)
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
}
