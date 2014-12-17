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

	template <typename t, int c> using vector = t[c];
	template <typename t, int r, int c> using matrix = vector<t, c>[r];

	using Bool = uint32;
	using Int = int32;
	using UInt = uint32;
	using DWord = uint32;
	using Float = float;

	enum
	{
		X = 0,
		Y = 1,
		Z = 2,
		W = 3
	};

	template <typename t> struct Vec1 { union { t m[1]; struct { t x; }; }; };
	template <typename t> struct Vec2 { union { t m[2]; struct { t x, y; }; }; };
	template <typename t> struct Vec3 { union { t m[3]; struct { t x, y, z; }; }; };
	template <typename t> struct Vec4 { union { t m[4]; struct { t x, y, z, w; }; }; };

	template <> struct Vec4<float>
	{
		union
		{
			float m[4];
			struct { float x, y, z, w; };
			__m128 m128;
		};

		Vec4()								{ }
		Vec4(__m128 v)						{ m128 = v; }
		Vec4<float> &operator = (__m128 v)	{ m128 = v; return *this; }
		operator __m128() const				{ return m128; }
	};

	#define Mat(T, C, R) struct join4(T, R, x, C)	\
	{												\
		union										\
		{											\
			struct									\
			{										\
				join2(Vec, C)<T> r[R];				\
			};										\
			T m[C][R];								\
		};											\
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

	def_vec(Bool)
	def_vec(Int)
	def_vec(UInt)
	def_vec(DWord)
	def_vec(Float)

	def_mat(Bool)
	def_mat(Int)
	def_mat(UInt)
	def_mat(DWord)
	def_mat(Float)

#pragma pop_macro("join2")
#pragma pop_macro("join3")
#pragma pop_macro("join4")
#pragma pop_macro("Mat")
#pragma pop_macro("def_vec")
#pragma pop_macro("def_mat_col")
#pragma pop_macro("def_mat")

#pragma pack(pop)

} // namespace HLSL
