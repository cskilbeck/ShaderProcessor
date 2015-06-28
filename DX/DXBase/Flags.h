#pragma once

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	template<typename T> struct Flags
	{
		Flags(T b = 0)
			: bits(b)
		{
		}

		operator T() const
		{
			return bits;
		}

		bool AreAllSet(T mask) const
		{
			return (bits & mask) == mask;
		}

		bool IsAnySet(T mask) const
		{
			return (bits & mask) != 0;
		}

		T Set(T mask)
		{
			return bits |= mask;
		}

		T Set(T mask, bool b)
		{
			return bits = (bits & ~mask) | (~((T)b - 1) & mask);
		}

		T Clear(T mask)
		{
			return bits &= ~mask;
		}

		bool operator() (T mask) const
		{
			return IsAnySet(mask);
		}

		T operator += (T mask)
		{
			return Set(mask);
		}

		T operator -= (T mask)
		{
			return Clear(mask);
		}

		T bits;
	};

	//////////////////////////////////////////////////////////////////////

	using Flags8 = Flags < uint8 > ;
	using Flags16 = Flags < uint16 > ;
	using Flags32 = Flags < uint32 > ;
	using Flags64 = Flags < uint64 > ;

}