//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

typedef __m128 Vec4f;

//////////////////////////////////////////////////////////////////////

struct ALIGNED(16) Vec4i
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

extern const __declspec(selectany) Vec4i gMMaskXYZ = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 };
extern const __declspec(selectany) Vec4i gMMaskX = { 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000 };
extern const __declspec(selectany) Vec4i gMMaskY = { 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000 };
extern const __declspec(selectany) Vec4i gMMaskZ = { 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000 };
extern const __declspec(selectany) Vec4i gMMaskW = { 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF };

//////////////////////////////////////////////////////////////////////

template<typename T> struct HLSLVec3;
template<typename T> struct HLSLVec4;

Vec4f		Vec4(float x, float y, float z);
Vec4f		Vec4(float x, float y, float z, float w);
Vec4f		Vec4(HLSLVec3<float> &f3);
Vec4f		Vec4(HLSLVec4<float> &f4);

Vec4f		SetX(Vec4f a, float x);
Vec4f		SetY(Vec4f a, float y);
Vec4f		SetZ(Vec4f a, float z);
Vec4f		SetW(Vec4f a, float w);

Vec4f		Splat(float n);

Vec4f		SplatX(Vec4f x);
Vec4f		SplatY(Vec4f y);
Vec4f		SplatZ(Vec4f z);
Vec4f		SplatW(Vec4f w);

Vec4f		MaskX(Vec4f x);
Vec4f		MaskY(Vec4f y);
Vec4f		MaskZ(Vec4f z);
Vec4f		MaskW(Vec4f w);

float		GetX(Vec4f v);
float		GetY(Vec4f v);
float		GetZ(Vec4f v);
float		GetW(Vec4f v);

Vec4f		GetX3(Vec4f a, Vec4f b, Vec4f c);
Vec4f		GetY3(Vec4f a, Vec4f b, Vec4f c);
Vec4f		GetZ3(Vec4f a, Vec4f b, Vec4f c);

Vec4f		GetXYZ(Vec4f x, Vec4f y, Vec4f z);

Vec4f		Negate(Vec4f v);
float		Dot(Vec4f a, Vec4f b);
float		LengthSquared(Vec4f v);
float		Length(Vec4f v);
Vec4f		Normalize(Vec4f v);
Vec4f		Cross(Vec4f a, Vec4f b);


Vec4f		operator + (Vec4f a, Vec4f b);
Vec4f		operator - (Vec4f a, Vec4f b);
Vec4f		operator * (Vec4f a, Vec4f b);
Vec4f		operator / (Vec4f a, Vec4f b);
Vec4f		operator * (Vec4f a, float b);
Vec4f		operator / (Vec4f a, float b);
Vec4f		operator * (float b, Vec4f a);
Vec4f		operator / (float b, Vec4f a);
Vec4f &		operator += (Vec4f &a, Vec4f b);
Vec4f &		operator -= (Vec4f &a, Vec4f b);
Vec4f &		operator *= (Vec4f &a, Vec4f b);
Vec4f &		operator *= (Vec4f &a, float b);
Vec4f &		operator /= (Vec4f &a, Vec4f b);
Vec4f &		operator /= (Vec4f &a, float b);

//////////////////////////////////////////////////////////////////////

