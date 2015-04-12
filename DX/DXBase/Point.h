//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	template<typename T> struct Point2: T
	{
		Point2()
		{
		}

		Point2(int x, int y)
		{
			this->x = x;
			this->y = y;
		}

		Point2(Point2 const &o)
		{
			x = o.x;
			y = o.y;
		}

		explicit Point2(Vec2f const &o)
		{
			x = (int)o.x;
			y = (int)o.y;
		}

		Point2(Size2D const &s)
		{
			x = s.cx;
			y = s.cy;
		}

		void Clear()
		{
			x = y = 0;
		}

		Point2 const &operator = (Point2 const &b)
		{
			x = b.x;
			y = b.y;
			return *this;
		}

		Point2 const &operator += (Point2 const &b)
		{
			x += b.x;
			y += b.y;
			return *this;
		}

		Point2 const &operator -= (Point2 const &b)
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

	template<typename T> inline bool operator == (Point2<T> const &a, Point2<T> const &b)
	{
		return a.x == b.x && a.y == b.y;
	}

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline Point2<T> operator + (Point2<T> const &a, Point2<T> const &b)
	{
		return Point2<T>(a.x + b.x, a.y + b.y);
	}

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline Point2<T> operator - (Point2<T> const &a, Point2<T> const &b)
	{
		return Point2<T>(a.x - b.x, a.y - b.y);
	}

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline Point2<T> operator - (Point2<T> const &a, Vec2f const &b)
	{
		return Point2<T>((int)(a.x - b.x), (int)(a.y - b.y));
	}

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline Point2<T> operator * (Point2<T> const &a, Point2<T> const &b)
	{
		return Point2<T>(a.x * b.x, a.y * b.y);
	}

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline Point2<T> operator * (Point2<T> const &a, int b)
	{
		return Point2<T>(a.x * b, a.y * b);
	}

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline Point2<T> operator / (Point2<T> const &a, Point2<T> const &b)
	{
		return Point2<T>(a.x / b.x, a.y / b.y);
	}

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline Point2<T> operator / (Point2<T> const &a, int b)
	{
		return Point2<T>(a.x / b, a.y / b);
	}

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline Point2<T> Sgn(Point2<T> a)
	{
		return Point2<T>(sgn(a.x), sgn(a.y));
	}

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline Point2<T> Min(Point2<T> a, Point2<T> b)
	{
		return Point2<T>(Min(a.x, b.x), Min(a.y, b.y));
	}

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline Point2<T> Max(Point2<T> a, Point2<T> b)
	{
		return Point2<T>(Max(a.x, b.x), Max(a.y, b.y));
	}

	//////////////////////////////////////////////////////////////////////

	using Point2D = Point2<POINT>;
	using Point2DS = Point2<POINTS>;

}

