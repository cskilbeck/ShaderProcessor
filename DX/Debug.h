//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct DXWindow;

	// Open/Close (alloc/free resources)

	HRESULT debug_open(DXWindow *w);
	void debug_close();

	// Begin/End frame

	void debug_begin();
	void debug_end();

	// Text

	void debug_text(char const *fmt, ...);
	void debug_text(wchar const *fmt, ...);
	void debug_text(int x, int y, char const *fmt, ...);
	void debug_text(int x, int y, wchar const *fmt, ...);

	// 3D

	void debug_set_camera(Camera const &camera);
	void debug_line(CVec4f start, CVec4f end, Color color);
	void debug_cube(CVec4f bottomLeft, CVec4f topRight, Color color);
	void debug_cylinder(CVec4f start, CVec4f end, float radius, Color color);
	void debug_cylinder(Matrix const &matrix, Color color);
	void debug_axes(CVec4f pos, float len = 1);
	void debug_dot(CVec4f pos, Color color);
	void debug_triangle(CVec4f a, CVec4f b, CVec4f c, Color color);
	void debug_quad(CVec4f a, CVec4f b, CVec4f c, CVec4f d, Color color);

	// 2D

	void debug_line2d(Vec2f const &a, Vec2f const &b, Color color);
	void debug_solid_rect2d(Vec2f const &topLeft, Vec2f const &bottomRight, Color color);
	void debug_outline_rect2d(Vec2f const &topLeft, Vec2f const &bottomRight, Color color);
	void debug_outline_solid_rect2d(Vec2f const &topLeft, Vec2f const &bottomRight, Color fillColor, Color lineColor);
}
