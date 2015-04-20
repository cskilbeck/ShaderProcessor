//////////////////////////////////////////////////////////////////////

namespace DX
{

#pragma push_macro("Permute")
#pragma push_macro("Permute2")
#undef Permute
#undef Permute2
#define Permute(d, c, b, a, v) _mm_shuffle_ps(v, v, ((d) << 6) | ((c) << 4) | ((b) << 2) | (a))
#define Permute2(d, c, b, a, v1, v2) _mm_shuffle_ps(v1, v2, ((d) << 6) | ((c) << 4) | ((b) << 2) | (a))

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Vec4(Float4 &f4)
	{
		return _mm_set_ps(f4.w, f4.z, f4.y, f4.z);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Vec4(Float3 &f3)
	{
		return _mm_set_ps(0.0f, f3.z, f3.y, f3.z);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Vec4(float x, float y, float z)
	{
		return _mm_set_ps(0.0f, z, y, x);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Vec4(float x, float y, float z, float w)
	{
		return _mm_set_ps(w, z, y, x);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f GetX3(CVec4f a, CVec4f b, CVec4f c)
	{
		Vec4f t = _mm_setzero_ps();
		Vec4f xy = Permute2(0, 0, 0, 0, a, b);
		Vec4f zw = Permute2(3, 3, 0, 0, c, t);
		Vec4f r = Permute2(2, 0, 2, 0, xy, zw);
		return r;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f GetY3(CVec4f a, CVec4f b, CVec4f c)
	{
		Vec4f t = _mm_setzero_ps();
		Vec4f xy = Permute2(1, 1, 1, 1, a, b);
		Vec4f zw = Permute2(3, 3, 1, 1, c, t);
		Vec4f r = Permute2(2, 0, 2, 0, xy, zw);
		return r;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f GetZ3(CVec4f a, CVec4f b, CVec4f c)
	{
		Vec4f t = _mm_setzero_ps();
		Vec4f xy = Permute2(2, 2, 2, 2, a, b);
		Vec4f zw = Permute2(3, 3, 2, 2, c, t);
		Vec4f r = Permute2(2, 0, 2, 0, xy, zw);
		return r;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f GetXYZ(CVec4f x, CVec4f y, CVec4f z)
	{
		Vec4f t = _mm_setzero_ps();
		Vec4f xy = Permute2(1, 1, 0, 0, x, y);
		Vec4f zw = Permute2(3, 3, 2, 2, z, t);
		Vec4f r = Permute2(2, 0, 2, 0, xy, zw);
		return r;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f SetX(CVec4f a, float x)
	{
		Vec4f r = _mm_set_ss(x);
		return _mm_move_ss(a, r);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f SetY(CVec4f a, float y)
	{
		Vec4f r = Permute(3, 2, 0, 1, a);
		Vec4f t = _mm_set_ss(y);
		r = _mm_move_ss(r, t);
		return Permute(3, 2, 0, 1, r);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f SetZ(CVec4f a, float z)
	{
		Vec4f r = Permute(3, 0, 1, 2, a);
		Vec4f t = _mm_set_ss(z);
		r = _mm_move_ss(r, t);
		return Permute(3, 0, 1, 2, r);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f SetW(CVec4f a, float w)
	{
		Vec4f r = Permute(0, 2, 1, 3, a);
		Vec4f t = _mm_set_ss(w);
		r = _mm_move_ss(r, t);
		return Permute(0, 2, 1, 3, r);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Select(CVec4f a, CVec4f b, Vec4i const &mask)
	{
		Vec4f t1 = _mm_andnot_ps(mask, a);
		Vec4f t2 = _mm_and_ps(b, mask);
		return _mm_or_ps(t1, t2);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Splat(float v)
	{
		return _mm_set_ps1(v);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f SplatX(CVec4f n)
	{
		return Permute(0, 0, 0, 0, n);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f SplatY(CVec4f y)
	{
		return Permute(1, 1, 1, 1, y);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f SplatZ(CVec4f z)
	{
		return Permute(2, 2, 2, 2, z);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f SplatW(CVec4f w)
	{
		return Permute(3, 3, 3, 3, w);
	}

	//////////////////////////////////////////////////////////////////////

	inline float GetX(CVec4f v)
	{
		return _mm_cvtss_f32(v);
	}

	//////////////////////////////////////////////////////////////////////

	inline float GetY(CVec4f v)
	{
		return _mm_cvtss_f32(Permute(1, 1, 1, 1, v));
	}

	//////////////////////////////////////////////////////////////////////

	inline float GetZ(CVec4f v)
	{
		return _mm_cvtss_f32(Permute(2, 2, 2, 2, v));
	}

	//////////////////////////////////////////////////////////////////////

	inline float GetW(CVec4f v)
	{
		return _mm_cvtss_f32(Permute(3, 3, 3, 3, v));
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f MaskX(CVec4f x)
	{
		return _mm_and_ps(x, gMMaskX);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f MaskY(CVec4f y)
	{
		return _mm_and_ps(y, gMMaskY);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f MaskZ(CVec4f z)
	{
		return _mm_and_ps(z, gMMaskZ);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f MaskW(CVec4f w)
	{
		return _mm_and_ps(w, gMMaskW);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Negate(CVec4f v)
	{
		return _mm_sub_ps(_mm_setzero_ps(), v);
	}

	//////////////////////////////////////////////////////////////////////

	inline float Dot(CVec4f a, CVec4f b)
	{
		Vec4f dot = _mm_mul_ps(a, b);
		Vec4f temp = Permute(2, 1, 2, 1, dot);
		dot = _mm_add_ss(dot, temp);
		temp = Permute(1, 1, 1, 1, temp);
		dot = _mm_add_ss(dot, temp);
		return GetX(dot);
	}

	//////////////////////////////////////////////////////////////////////

	inline float LengthSquared(Vec4f v)
	{
		return Dot(v, v);
	}

	//////////////////////////////////////////////////////////////////////

	inline float Length(CVec4f v)
	{
		Vec4f l = _mm_mul_ps(v, v);
		Vec4f t = Permute(2, 1, 2, 1, l);
		l = _mm_add_ss(l, t);
		t = Permute(1, 1, 1, 1, t);
		l = _mm_add_ss(l, t);
		return GetX(_mm_sqrt_ps(l));
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Normalize(CVec4f v)
	{
		Vec4f ls = _mm_mul_ps(v, v);
		Vec4f t = Permute(2, 1, 2, 1, ls);
		ls = _mm_add_ss(ls, t);
		t = Permute(1, 1, 1, 1, t);
		ls = _mm_add_ss(ls, t);
		ls = Permute(0, 0, 0, 0, ls);
		return _mm_div_ps(v, _mm_sqrt_ps(ls));
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Cross(CVec4f a, CVec4f b)
	{
		Vec4f t1 = Permute(3, 0, 2, 1, a);
		Vec4f t2 = Permute(3, 1, 0, 2, b);
		Vec4f r = _mm_mul_ps(t1, t2);
		t1 = Permute(3, 0, 2, 1, t1);
		t2 = Permute(3, 1, 0, 2, t2);
		t1 = _mm_mul_ps(t1, t2);
		r = _mm_sub_ps(r, t1);
		return _mm_and_ps(r, gMMaskXYZ);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Average(CVec4f a, CVec4f b)
	{
		return _mm_add_ps(a, b) * 0.5f;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f operator / (CVec4f a, CVec4f b)
	{
		return _mm_div_ps(a, b);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f operator * (CVec4f a, float b)
	{
		return _mm_mul_ps(a, _mm_set_ps(b, b, b, b));
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f operator / (CVec4f a, float b)
	{
		return _mm_div_ps(a, _mm_set_ps(b, b, b, b));
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f operator * (float b, CVec4f a)
	{
		return _mm_mul_ps(a, _mm_set_ps(b, b, b, b));
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f operator / (float b, CVec4f a)
	{
		return _mm_div_ps(a, _mm_set_ps(b, b, b, b));
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f &operator += (Vec4f &a, CVec4f b)
	{
		a = _mm_add_ps(a, b);
		return a;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f &operator -= (Vec4f &a, CVec4f b)
	{
		a = _mm_sub_ps(a, b);
		return a;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f &operator *= (Vec4f &a, CVec4f b)
	{
		a = _mm_mul_ps(a, b);
		return a;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f &operator *= (Vec4f &a, float b)
	{
		a = _mm_mul_ps(a, _mm_set_ps(b, b, b, b));
		return a;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f &operator /= (Vec4f &a, CVec4f b)
	{
		a = _mm_div_ps(a, b);
		return a;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f &operator /= (Vec4f &a, float b)
	{
		a = _mm_div_ps(a, _mm_set_ps(b, b, b, b));
		return a;
	}

	//////////////////////////////////////////////////////////////////////

#pragma pop_macro("Permute")
#pragma pop_macro("Permute2")

}