//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

class btVector3;

namespace DX
{
	typedef DirectX::XMVECTOR Vec4f;
	typedef DirectX::FXMVECTOR CVec4f;

	//////////////////////////////////////////////////////////////////////

	struct __declspec(align(16)) Vec4i
	{
		union
		{
			uint32 i[4];
			Vec4f v;
		};

		operator Vec4f () const
		{
			return v;
		}
	};

	extern const __declspec(align(16)) __declspec(selectany) Vec4i gMMaskXYZ = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 };
	extern const __declspec(align(16)) __declspec(selectany) Vec4i gMMaskX = { 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000 };
	extern const __declspec(align(16)) __declspec(selectany) Vec4i gMMaskY = { 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000 };
	extern const __declspec(align(16)) __declspec(selectany) Vec4i gMMaskZ = { 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000 };
	extern const __declspec(align(16)) __declspec(selectany) Vec4i gMMaskW = { 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF };

	//////////////////////////////////////////////////////////////////////

	template<typename T> struct HLSLVec3;
	template<typename T> struct HLSLVec4;

	struct Vec2f;

	Vec4f		Vec4(float x, float y, float z);
	Vec4f		Vec4(float x, float y, float z, float w);
	Vec4f		Vec4(HLSLVec3<float> const &f3);
	Vec4f		Vec4(HLSLVec4<float> const &f4);
	Vec4f		Vec4(Vec2f const &v);

	Vec4f		V4Add(CVec4f a, CVec4f b);
	Vec4f		V4Sub(CVec4f a, CVec4f b);

	Vec4f		SetX(CVec4f a, float x);
	Vec4f		SetY(CVec4f a, float y);
	Vec4f		SetZ(CVec4f a, float z);
	Vec4f		SetW(CVec4f a, float w);

	Vec4f		Select(CVec4f a, CVec4f b, Vec4i const &mask);

	Vec4f		Splat(float n);

	Vec4f		SplatX(CVec4f x);
	Vec4f		SplatY(CVec4f y);
	Vec4f		SplatZ(CVec4f z);
	Vec4f		SplatW(CVec4f w);

	Vec4f		MaskX(CVec4f x);
	Vec4f		MaskY(CVec4f y);
	Vec4f		MaskZ(CVec4f z);
	Vec4f		MaskW(CVec4f w);

	float		GetX(CVec4f v);
	float		GetY(CVec4f v);
	float		GetZ(CVec4f v);
	float		GetW(CVec4f v);

	Vec4f		GetX3(CVec4f a, CVec4f b, CVec4f c);
	Vec4f		GetY3(CVec4f a, CVec4f b, CVec4f c);
	Vec4f		GetZ3(CVec4f a, CVec4f b, CVec4f c);

	Vec4f		GetXYZ(CVec4f x, CVec4f y, CVec4f z);

	Vec4f		Negate(CVec4f v);
	float		Dot(CVec4f a, CVec4f b);
	float		LengthSquared(CVec4f v);
	float		Length(CVec4f v);
	Vec4f		Normalize(CVec4f v);
	Vec4f		Cross(CVec4f a, CVec4f b);
	Vec4f		AxisAngleFromQuaternion(CVec4f quat);
	Vec4f		QuaternionFromAxisAngle(CVec4f axisAngle);

	Vec4f		operator - (CVec4f a);

	Vec4f		operator / (CVec4f a, CVec4f b);
	Vec4f		operator * (CVec4f a, float b);
	Vec4f		operator / (CVec4f a, float b);
	Vec4f		operator * (float b, CVec4f a);
	Vec4f		operator / (float b, CVec4f a);
	Vec4f &		operator += (Vec4f &a, CVec4f b);
	Vec4f &		operator -= (Vec4f &a, CVec4f b);
	Vec4f &		operator *= (Vec4f &a, CVec4f b);
	Vec4f &		operator *= (Vec4f &a, float b);
	Vec4f &		operator /= (Vec4f &a, CVec4f b);
	Vec4f &		operator /= (Vec4f &a, float b);

	//////////////////////////////////////////////////////////////////////

#pragma push_macro("Permute")
#pragma push_macro("Permute2")
#pragma push_macro("X")
#pragma push_macro("Y")
#pragma push_macro("Z")
#pragma push_macro("W")
#undef Permute
#undef Permute2
#define Permute(d, c, b, a, v) _mm_shuffle_ps(v, v, ((d) << 6) | ((c) << 4) | ((b) << 2) | (a))
#define Permute2(d, c, b, a, v1, v2) _mm_shuffle_ps(v1, v2, ((d) << 6) | ((c) << 4) | ((b) << 2) | (a))
#define X 0
#define Y 1
#define Z 2
#define W 3

	//////////////////////////////////////////////////////////////////////
	// This one with member functions and field access

	struct v4;
	using v4c = v4 const &;

	__declspec(align(16)) struct v4
	{
		union
		{
			struct
			{
				float x, y, z, w;
			};
			Vec4f v;
		};

		v4()
		{
		}

		v4(Vec2f const &a);

		v4(CVec4f c)
			: v(c)
		{
		}

		v4(float x, float y, float z)
		{
			v = _mm_set_ps(0.0f, z, y, x);
		}

		v4(float x, float y, float z, float w)
		{
			v = _mm_set_ps(w, z, y, x);
		}

		operator Vec4f const () const
		{
			return v;
		}

		operator Vec4f()
		{
			return v;
		}

		float Dot(v4c a) const
		{
			return _mm_cvtss_f32(_mm_dp_ps(v, a.v, 0xff));
		}

		v4 Cross(v4c a)
		{
			v4 t1 = Permute(W, X, Z, Y, v);
			v4 t2 = Permute(W, Y, X, Z, a.v);
			v4 r = _mm_mul_ps(t1, t2);
			t1 = Permute(W, X, Z, Y, t1);
			t2 = Permute(W, Y, X, Z, t2);
			t1 = _mm_mul_ps(t1, t2);
			r = _mm_sub_ps(r, t1);
			return _mm_and_ps(r, gMMaskXYZ);
		}

		v4 Normalized() const
		{
			v4 ls = _mm_mul_ps(v, v);
			v4 t = Permute(Z, Y, Z, Y, ls);
			ls = _mm_add_ss(ls, t);
			t = Permute(Y, Y, Y, Y, t);
			ls = _mm_add_ss(ls, t);
			ls = Permute(X, X, X, X, ls);
			return _mm_div_ps(v, _mm_sqrt_ps(ls));
		}

		float Normalize()
		{
			v4 ls = _mm_mul_ps(v, v);
			v4 t = Permute(Z, Y, Z, Y, ls);
			ls = _mm_add_ss(ls, t);
			t = Permute(Y, Y, Y, Y, t);
			ls = _mm_add_ss(ls, t);
			ls = Permute(X, X, X, X, ls);
			v4 l = _mm_sqrt_ps(ls);
			v = _mm_div_ps(v, l);
			return GetX(l);
		}

		float LengthSquared() const
		{
			return _mm_cvtss_f32(_mm_dp_ps(v, v, 0xff));
		}

		float Length() const
		{
			return sqrtf(_mm_cvtss_f32(_mm_dp_ps(v, v, 0xff)));
		}

		static inline v4 zero()
		{
			return _mm_setzero_ps();
		}
	};

#pragma pop_macro("Permute")
#pragma pop_macro("Permute2")
#pragma pop_macro("X")
#pragma pop_macro("Y")
#pragma pop_macro("Z")
#pragma pop_macro("W")

}