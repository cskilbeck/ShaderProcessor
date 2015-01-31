//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////
// Define Vector and Matrix types which correspond to HLSL types (mostly, kinda)

namespace HLSL
{
#pragma pack(push, 4)

#pragma push_macro("join2")
#pragma push_macro("join3")
#pragma push_macro("join4")
#pragma push_macro("Mat")
#pragma push_macro("def_vec")
#pragma push_macro("def_mat_col")
#pragma push_macro("def_mat")

#define join2(x, y) x ## y
#define join3(x, y, z) join2(x, y) ## z
#define join4(x, y, z, w) join3(x, y, z) ## w

	using Byte = uint8;
	using SByte = int8;
	using Short = int16;
	using UShort = uint16;
	using Int = int32;
	using UInt = uint32;
	using Float = float;
	using Void = uint32;
	using Half = half;

	enum
	{
		X = 0,
		Y = 1,
		Z = 2,
		W = 3
	};

	template <typename t> struct Vec1 { t x; Vec1() {} Vec1(t x): x(x) { } };
	template <typename t> struct Vec2 { t x, y; Vec2() {} Vec2(t x, t y): x(x), y(y) { } };
	template <typename t> struct Vec3 { t x, y, z; Vec3() {} Vec3(t x, t y, t z): x(x), y(y), z(z) { } };
	template <typename t> struct Vec4 { t x, y, z, w; Vec4() {} Vec4(t x, t y, t z, t w): x(x), y(y), z(z), w(w) { } };

	template <> struct ALIGNED(16) Vec4<float>
	{
		union
		{
			float m[4];
			struct { float x, y, z, w; };
			__m128 m128;
		};

		Vec4()								{ }
		Vec4(Vec4<float> const &o)			{ m128 = o.m128; }
		Vec4(__m128 v)						{ m128 = v; }
		Vec4<float> &operator = (__m128 v)	{ m128 = v; return *this; }
		operator __m128() const				{ return m128; }
	};

	#define Mat(T, C, R) struct join4(T, R, x, C)	\
	{												\
		T m[C][R];									\
	};

	#define def_vec(T)							\
		using join2(T, 1) = join2(Vec, 1)<T>;	\
		using join2(T, 2) = join2(Vec, 2)<T>;	\
		using join2(T, 3) = join2(Vec, 3)<T>;	\
		using join2(T, 4) = join2(Vec, 4)<T>;

	#define def_mat_col(T, C)	\
		Mat(T, C, 1);			\
		Mat(T, C, 2);			\
		Mat(T, C, 3);			\
		Mat(T, C, 4);

	#define def_mat(T)		\
		def_mat_col(T, 1)	\
		def_mat_col(T, 2)	\
		def_mat_col(T, 3)	\
		def_mat_col(T, 4)

	def_vec(Float)
	def_vec(Half)
	def_vec(UInt)
	def_vec(Int)
	def_vec(UShort)
	def_vec(Short)
	def_vec(Byte)
	def_vec(SByte)
	def_vec(Void)

	def_mat(Float)
	def_mat(Half)
	def_mat(UInt)
	def_mat(Int)
	def_mat(UShort)
	def_mat(Short)
	def_mat(Byte)
	def_mat(SByte)
	def_mat(Void)

#pragma pop_macro("join2")
#pragma pop_macro("join3")
#pragma pop_macro("join4")
#pragma pop_macro("Mat")
#pragma pop_macro("def_vec")
#pragma pop_macro("def_mat_col")
#pragma pop_macro("def_mat")

#pragma pack(pop)

} // namespace HLSL
