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

	void debug_line(Vec4f start, Vec4f end, Color color);
	void debug_cube(Vec4f bottomLeft, Vec4f topRight, Color color);
	void debug_cylinder(Vec4f start, Vec4f end, float radius, Color color);
	void debug_cylinder(Matrix const &matrix, Color color);
}
