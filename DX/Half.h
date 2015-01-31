//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct half
{
	half()
	{
	}

	half(float flt)
	{
		__m128 V1 = _mm_set_ss(flt);
		__m128i V2 = _mm_cvtps_ph(V1, 0);
		n = (uint16)_mm_cvtsi128_si32(V2);
	}

	operator float() const
	{
		__m128i V1 = _mm_cvtsi32_si128(n);
		__m128 V2 = _mm_cvtph_ps(V1);
		return _mm_cvtss_f32(V2);
	}

	uint16 n;
};
