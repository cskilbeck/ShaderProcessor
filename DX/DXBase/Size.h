//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	template <typename T> struct Size2: T
	{
		Size2()
		{
		}

		Size2(int w, int h)
		{
			cx = w;
			cy = h;
		}

		Size2(Size2 const &o)
		{
			cx = o.cx;
			cy = o.cy;
		}

		explicit Size2(Vec2f const &o)
		{
			cx = (int)o.x;
			cy = (int)o.y;
		}

		explicit Size2(Point2D const &o)
		{
			cx = o.x;
			cy = o.y;
		}

		long Width() const
		{
			return cx;
		}

		long Height() const
		{
			return cy;
		}

		void Clear()
		{
			cx = 0;
			cy = 0;
		}

		bool operator < (Size2 const &o) const
		{
			return (cx == o.cx) ? (cy < o.cy) : (cx < o.cx);
		}
	};

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline Point2D operator + (Point2D a, Size2<T> b)
	{
		return Point2D(a.x + b.cx, a.y + b.cy);
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline Size2<T> operator + (Size2<T> a, Size2<T> b)
	{
		return Size2<T>(a.cx + b.cx, a.cy + b.cy);
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline Size2<T> operator + (Size2<T> a, Point2D b)
	{
		return Size2<T>(a.cx + b.x, a.cy + b.y);
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline Point2D operator - (Point2D a, Size2<T> b)
	{
		return Point2D(a.x - b.cx, a.y - b.cy);
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline Size2<T> operator - (Size2<T> a, Size2<T> b)
	{
		return Size2<T>(a.cx - b.cx, a.cy - b.cy);
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline Size2<T> operator - (Size2<T> a, Point2D b)
	{
		return Size2<T>(a.cx - b.x, a.cy - b.y);
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline Size2<T> operator - (Size2<T> const &s, Vec2f const &v)
	{
		return Size2<T>((long)(s.cx - v.x), (long)(s.cy - v.y));
	}

	template <typename T> inline bool operator == (Size2<T> const &a, Size2<T> const &b)
	{
		return a.cx == b.cx && a.cy == b.cy;
	}

	using Size2D = Size2<SIZE>;

}