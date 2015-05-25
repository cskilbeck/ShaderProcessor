//////////////////////////////////////////////////////////////////////

#pragma once

#include <math.h>

//////////////////////////////////////////////////////////////////////

namespace DX
{
	template<typename T> struct Point2;
	template<typename T> struct Size2;
	using Size2D = Size2<SIZE>;

	//////////////////////////////////////////////////////////////////////

#pragma pack(push, 4)

	struct Vec2f
	{
		float x;
		float y;

		static Vec2f one;
		static Vec2f zero;
		static Vec2f half;

		Vec2f()
		{
		}

		Vec2f(float _x, float _y) : x(_x), y(_y)
		{
		}
		
		Vec2f(Float2 f) : x(f.x), y(f.y)
		{
		}

		Vec2f(Vec2f const &o)
		{
			x = o.x;
			y = o.y;
		}

		operator Float2()
		{
			return Float2(x, y);
		}

		explicit Vec2f(Point2<POINT> const &p);
		explicit Vec2f(Point2<POINTS> const &p);
		explicit Vec2f(Size2D const &s);

		Vec2f const &operator = (Vec2f const &b)
		{
			x = b.x;
			y = b.y;
			return *this;
		}

		Vec2f const &operator += (Vec2f const &b)
		{
			x += b.x;
			y += b.y;
			return *this;
		}

		Vec2f const &operator -= (Vec2f const &b)
		{
			x -= b.x;
			y -= b.y;
			return *this;
		}

		Vec2f const &operator *= (Vec2f const &b)
		{
			x *= b.x;
			y *= b.y;
			return *this;
		}

		Vec2f const &operator /= (Vec2f const &b)
		{
			x /= b.x;
			y /= b.y;
			return *this;
		}

		Vec2f const &operator *= (float f)
		{
			x *= f;
			y *= f;
			return *this;
		}

		Vec2f const &operator /= (float f)
		{
			x /= f;
			y /= f;
			return *this;
		}

		Vec2f operator - ()
		{
			return Vec2f(-x, -y);
		}

		void Set(float x, float y)
		{
			this->x = x;
			this->y = y;
		}

		void Reset()
		{
			x = y = 0;
		}

		float Dot(Vec2f const &o) const
		{
			return x * o.x + y * o.y;
		}

		float Cross(Vec2f const &o) const
		{
			return x * o.y - y * o.x;
		}

		float LengthSquared() const
		{
			return x * x + y * y;
		}

		float Length() const
		{
			return sqrtf(LengthSquared());
		}

		operator DirectX::XMVECTOR() const
		{
			return DirectX::XMLoadFloat2((CONST DirectX::XMFLOAT2*)this);
		}

		operator DirectX::XMFLOAT2() const
		{
			return DirectX::XMFLOAT2(x, y);
		}

		Vec2f &operator = (Point2<POINT> const &p);
		Vec2f operator - (Size2D const &s);

		string ToString() const;

		Vec2f Normalize();
	};

#pragma pack(pop)

	//////////////////////////////////////////////////////////////////////

	inline Vec2f operator - (Vec2f const &a)
	{
		return Vec2f(-a.x, -a.y);
	}

	//////////////////////////////////////////////////////////////////////

	inline bool operator <= (Vec2f const &a, Vec2f const &b)
	{
		return a.x <= b.x && a.y <= b.y;
	}

	//////////////////////////////////////////////////////////////////////

	inline bool operator >= (Vec2f const &a, Vec2f const &b)
	{
		return a.x >= b.x && a.y >= b.y;
	}

	//////////////////////////////////////////////////////////////////////

	inline bool operator < (Vec2f const &a, Vec2f const &b)
	{
		return a.x < b.x && a.y < b.y;
	}

	//////////////////////////////////////////////////////////////////////

	inline bool operator >(Vec2f const &a, Vec2f const &b)
	{
		return a.x > b.x && a.y > b.y;
	}

	//////////////////////////////////////////////////////////////////////

	inline bool operator == (Vec2f const &l, Vec2f const &r)
	{
		return l.x == r.x && l.y == r.y;
	}

	//////////////////////////////////////////////////////////////////////

	inline bool operator != (Vec2f const &l, Vec2f const &r)
	{
		return l.x != r.x || l.y != r.y;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec2f operator + (Vec2f const &a, Vec2f const &b)
	{
		return Vec2f(a.x + b.x, a.y + b.y);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec2f operator - (Vec2f const &a, Vec2f const &b)
	{
		return Vec2f(a.x - b.x, a.y - b.y);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec2f operator * (Vec2f const &a, Vec2f const &b)
	{
		return Vec2f(a.x * b.x, a.y * b.y);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec2f operator * (Vec2f const &a, float b)
	{
		return Vec2f(a.x * b, a.y * b);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec2f operator / (Vec2f const &a, Vec2f const &b)
	{
		return Vec2f(a.x / b.x, a.y / b.y);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec2f operator / (Vec2f const &a, float f)
	{
		return Vec2f(a.x / f, a.y / f);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec2f Vec2f::Normalize()
	{
		return *this / Length();
	}

	//////////////////////////////////////////////////////////////////////

	template <> inline Vec2f Min(Vec2f a, Vec2f b)
	{
		return Vec2f(Min(a.x, b.x), Min(a.y, b.y));
	}

	//////////////////////////////////////////////////////////////////////

	template <> inline Vec2f Max(Vec2f a, Vec2f b)
	{
		return Vec2f(Max(a.x, b.x), Max(a.y, b.y));
	}
}

