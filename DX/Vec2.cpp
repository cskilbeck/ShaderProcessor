//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

Vec2 Vec2::one(1, 1);
Vec2 Vec2::zero(0, 0);
Vec2 Vec2::half(0.5f, 0.5f);

//////////////////////////////////////////////////////////////////////

Vec2::Vec2(Point2D const &p) : x((float)p.x), y((float)p.y)
{
}

Vec2::Vec2(Size2D const &s) : x((float)s.Width()), y((float)s.Height())
{
}

Vec2 Vec2::operator - (Size2D const &s)
{
	return Vec2(x - s.cx, y - s.cy);
}

Vec2 &Vec2::operator = (Point2D const &p)
{
	x = (float)p.x;
	y = (float)p.y;
	return *this;
}
