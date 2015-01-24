//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct half
{
	half();
	half(float flt);
	operator float() const;

private:

	static uint16 basetable[512];
	static uint8 shifttable[512];
	static uint32 mantissatable[2048];
	static uint16 offsettable[64];
	static uint32 exponenttable[64];

	uint16 n;
};

//////////////////////////////////////////////////////////////////////

inline half::half()
{
}

//////////////////////////////////////////////////////////////////////

inline half::half(float flt)
{
	uint32 f = *(uint32 *)&flt;
	uint32 f23 = f >> 23;
	n = basetable[(f23) & 0x1ff] + ((f & 0x007fffff) >> shifttable[(f23) & 0x1ff]);}

//////////////////////////////////////////////////////////////////////

inline half::operator float() const
{
	uint32 n10 = n >> 10;
	uint32 v = mantissatable[offsettable[n10] + (n & 0x3ff)] + exponenttable[n10];
	return *(float *)&v;
}
