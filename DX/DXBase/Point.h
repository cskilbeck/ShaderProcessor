//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{

	struct Point2D: POINT
	{
		Point2D()
		{
		}

		Point2D(int x, int y)
		{
			this->x = x;
			this->y = y;
		}

		Point2D(Point2D const &o)
		{
			x = o.x;
			y = o.y;
		}

		explicit Point2D(Vec2f const &o)
		{
			x = (int)o.x;
			y = (int)o.y;
		}

		void Clear()
		{
			x = y = 0;
		}

		Point2D const &operator = (Point2D const &b)
		{
			x = b.x;
			y = b.y;
			return *this;
		}

		Point2D const &operator += (Point2D const &b)
		{
			x += b.x;
			y += b.y;
			return *this;
		}

		Point2D const &operator -= (Point2D const &b)
		{
			x -= b.x;
			y -= b.y;
			return *this;
		}

		tstring ToString() const
		{
			return Format(TEXT("(x=%d,y=%d)"), x, y);
		}
	};

	//////////////////////////////////////////////////////////////////////

	inline bool operator == (Point2D const &a, Point2D const &b)
	{
		return a.x == b.x && a.y == b.y;
	}

	//////////////////////////////////////////////////////////////////////

	inline Point2D operator + (Point2D const &a, Point2D const &b)
	{
		return Point2D(a.x + b.x, a.y + b.y);
	}

	//////////////////////////////////////////////////////////////////////

	inline Point2D operator - (Point2D const &a, Point2D const &b)
	{
		return Point2D(a.x - b.x, a.y - b.y);
	}

	//////////////////////////////////////////////////////////////////////

	inline Point2D operator - (Point2D const &a, Vec2f const &b)
	{
		return Point2D((int)(a.x - b.x), (int)(a.y - b.y));
	}

	//////////////////////////////////////////////////////////////////////

	inline Point2D operator * (Point2D const &a, Point2D const &b)
	{
		return Point2D(a.x * b.x, a.y * b.y);
	}

	//////////////////////////////////////////////////////////////////////

	inline Point2D operator * (Point2D const &a, int b)
	{
		return Point2D(a.x * b, a.y * b);
	}

	//////////////////////////////////////////////////////////////////////

	inline Point2D operator / (Point2D const &a, Point2D const &b)
	{
		return Point2D(a.x / b.x, a.y / b.y);
	}

	//////////////////////////////////////////////////////////////////////

	inline Point2D operator / (Point2D const &a, int b)
	{
		return Point2D(a.x / b, a.y / b);
	}

	//////////////////////////////////////////////////////////////////////

	inline Point2D Sgn(Point2D a)
	{
		return Point2D(sgn(a.x), sgn(a.y));
	}

	//////////////////////////////////////////////////////////////////////

	template <> inline Point2D Min(Point2D a, Point2D b)
	{
		return Point2D(Min(a.x, b.x), Min(a.y, b.y));
	}

	//////////////////////////////////////////////////////////////////////

	template <> inline Point2D Max(Point2D a, Point2D b)
	{
		return Point2D(Max(a.x, b.x), Max(a.y, b.y));
	}

}

