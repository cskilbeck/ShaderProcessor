//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct DXWindow;

	HRESULT debug_open(DXWindow *w);
	void debug_close();

	void debug_begin(Camera &camera);
	void debug_end();

	void debug_text(char const *fmt, ...);
	void debug_text(wchar const *fmt, ...);
	void debug_text(int x, int y, char const *fmt, ...);
	void debug_text(int x, int y, wchar const *fmt, ...);

	void debug_line(CVec4f start, CVec4f end, Color color);
	void debug_cube(CVec4f bottomLeft, CVec4f topRight, Color color);
	void debug_cylinder(CVec4f start, CVec4f end, float radius, Color color);
	void debug_cylinder(Matrix const &matrix, Color color);

	void debug_axes(CVec4f pos, float len = 1);
	void debug_dot(CVec4f pos, Color color);

	void debug_triangle(CVec4f a, CVec4f b, CVec4f c, Color color);
	void debug_quad(CVec4f a, CVec4f b, CVec4f c, CVec4f d, Color color);
}
