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

	Vec4f		Vec4(float x, float y, float z);
	Vec4f		Vec4(float x, float y, float z, float w);
	Vec4f		Vec4(HLSLVec3<float> const &f3);
	Vec4f		Vec4(HLSLVec4<float> const &f4);

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

}