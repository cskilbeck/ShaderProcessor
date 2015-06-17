//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	string Format_V(char const *format, va_list v);
	wstring Format_V(wchar const *format, va_list v);

	void Trace(wchar const *strMsg, ...);
	void Trace(char const *strMsg, ...);

#define TRACE_ON Trace
#define TRACE_OFF(...) if (false) {} else (__VA_ARGS__);

#if defined(_DEBUG)
#	define ERR(...)		TRACE_ON(__VA_ARGS__)
#	define TRACE(...)	TRACE_ON(__VA_ARGS__)
#	define WARN(...)	TRACE_ON(__VA_ARGS__)
#	define INFO(...)	TRACE_OFF(__VA_ARGS__)
#else
#	define ERR(...)		TRACE_OFF(__VA_ARGS__)
#	define TRACE(...)	TRACE_OFF(__VA_ARGS__)
#	define WARN(...)	TRACE_OFF(__VA_ARGS__)
#	define INFO(...)	TRACE_OFF(__VA_ARGS__)
#endif

	void ErrorMessageBox(tchar const *msg, ...);

//	HRESULT LoadResource(uint32 resourceID, void **data, size_t *size = null);

	wstring WideStringFromTString(tstring const &str);
	wstring WideStringFromString(string const &str);

	tstring TStringFromWideString(wstring const &str);
	tstring TStringFromString(string const &str);

	string StringFromTString(tstring const &str);
	string StringFromWideString(wstring const &str);

	wstring Format(wchar const *fmt, ...);
	string Format(char const *fmt, ...);

	tstring ConcatenatePaths(tchar const *base, tchar const *relative);
	tstring GetCurrentFolder();
	tstring GetDrive(tchar const *path);
	tstring GetPath(tchar const *path);
	tstring GetFilename(tchar const *path);
	tstring GetExtension(tchar const *path);
	tstring SetExtension(tchar const *path, tchar const *ext);

	struct Vec2f;

	bool LineIntersect(Vec2f const &a1, Vec2f const &a2, Vec2f const &b1, Vec2f const &b2, Vec2f *intersectionPoint);

	bool PointInRectangle(Vec2f const &point, Vec2f const r[4]);
	bool PointInRectangle(Vec2f const &point, Vec2f const r[4], float const margins[4]);

	bool RectanglesOverlap(Vec2f const a[4], Vec2f const b[4]);

#ifndef PI
#	define PI 3.14159265f
#	define PI_2 (PI/2)
#endif

	//////////////////////////////////////////////////////////////////////
	// UTF8 decoding

	bool GetUTF8Length(uint8 *bytes, uint32 *length);
	bool DecodeUTF8(uint8 *bytes, wchar *buffer, uint32 bufferSize);

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline int64 asciiToInt64(T const *p)
	{
		assert(p != 0);
		int64 result = 0;
		bool negate = false;
		if(*p == '-')
		{
			negate = true;
			++p;
		}
		while(*p)
		{
			result *= 10;
			T c = *p++;
			if(c < '0' || c > '9')
			{
				break;
			}
			result += c - '0';
		}
		return negate ? -result : result;
	}

	//////////////////////////////////////////////////////////////////////

	template<typename T> T ReplaceAll(T &str, T const &from, T const &to)
	{
		size_t start_pos = 0;
 		while((start_pos = str.find(from, start_pos)) != T::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
		return str;
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> void Delete(T * &arr)
	{
		if(arr != null)
		{
			delete arr;
			arr = null;
		}
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> void Release(T * &ptr)
	{
		if(ptr != null)
		{
			ptr->Release();
			ptr = null;
		}
	}

	//////////////////////////////////////////////////////////////////////
	// !! BY VALUE

	template <typename T> T Max(T a, T b)
	{
		return a > b ? a : b;
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> T Min(T a, T b)
	{
		return a < b ? a : b;
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> T Constrain(T a, T lower, T upper)
	{
		return Min(Max(a, lower), upper);
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> int sgn(T val)	// where T is int, short, char etc. caveat caller
	{
		return (T(0) < val) - (val < T(0));
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> void Swap(T &a, T &b)
	{
		std::swap(a, b);
	}

	//////////////////////////////////////////////////////////////////////

	inline float Ease(float d)
	{
		float d2 = d * d;
		float d3 = d2 * d;
		return 3 * d2 - 2 * d3;
	}

	//////////////////////////////////////////////////////////////////////

	inline string ToLower(string const &s)
	{
		string r(s);
		std::transform(r.begin(), r.end(), r.begin(), ::tolower);
		return r;
	}

	//////////////////////////////////////////////////////////////////////

	inline wstring ToLower(wstring const &s)
	{
		wstring r(s);
		std::transform(r.begin(), r.end(), r.begin(), ::tolower);
		return r;
	}

	//////////////////////////////////////////////////////////////////////

	inline int CountBits(uint32 i)
	{
		i = i - ((i >> 1) & 0x55555555);
		i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
		return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
	}

	//////////////////////////////////////////////////////////////////////

	inline int HighBit(uint32 x)
	{
		unsigned long index;
		Byte isNonzero = _BitScanReverse(&index, x);
		return isNonzero ? index + 1 : 0;
	}

	//////////////////////////////////////////////////////////////////////

#if defined(_WIN64)
	inline int HighBit64(uint64 x)
	{
		unsigned long index;
		Byte isNonzero = _BitScanReverse64(&index, x);
		return isNonzero ? index + 1 : 0;
	}
#endif

	//////////////////////////////////////////////////////////////////////

	inline int NextPowerOf2(int v)
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}

	//////////////////////////////////////////////////////////////////////

	template<class T> struct in_reverse
	{
		T &l;

		in_reverse(T &list) : l(list)
		{
		}

		auto begin() -> decltype(l.rbegin())
		{
			return l.rbegin();
		}
		auto begin() const -> decltype(l.crbegin())
		{
			return l.crbegin();
		}
		auto end() -> decltype(l.rend())
		{
			return l.rend();
		}
		auto end() const -> decltype(l.crend())
		{
			return l.crend();
		}
	};

	template<class T> in_reverse<T> reverse(T &l)
	{
		return in_reverse<T>(l);
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> size_t Length(T const *str)
	{
		assert(str != null);

		T const *p = str;
		while(*p++)
		{
		}
		return p - str;
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> T const *FindFirstOf(T const *str, T const *find, size_t numDelimiters)
	{
		assert(str != null);
		assert(find != null);

		T ch;
		while((ch = *str))
		{
			for(size_t i = 0; i < numDelimiters; ++i)
			{
				if(ch == find[i])
				{
					return str;
				}
			}
			++str;
		}
		return null;
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> T const *FindFirstOf(T const *str, T const *find)
	{
		return FindFirstOf(str, find, Length(find));
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> std::basic_string<T> Trim(std::basic_string<T> str)
	{
		str.erase(0, str.find_first_not_of(" \t\r\n"));
		str.erase(str.find_last_not_of(" \t\r\n") + 1);
		return str;
	}

	//////////////////////////////////////////////////////////////////////

	template <class container_t, class string_t, class char_t>
	void tokenize(string_t const &str, container_t &tokens, char_t const *delimiters, bool includeEmpty = true)
	{
		string_t::size_type end = 0, start = 0, len = str.size();
		while(end < len)
		{
			end = str.find_first_of(delimiters, start);
			if(end == string_t::npos)
			{
				end = len;
			}
			if(end != start || includeEmpty)
			{
				tokens.push_back(container_t::value_type(str.data() + start, end - start));
			}
			start = end + 1;
		}
	}

	//////////////////////////////////////////////////////////////////////

	template <class container_t, class char_t>
	void tokenize(char_t const *str, container_t &tokens, char_t const *delimiters, bool includeEmpty = true)
	{
		assert(str != null);
		assert(delimiters != null);

		size_t delimCount = Length(delimiters);
		size_t len = Length(str);	// LAME
		char_t const *start = str, *end = str, *stop = start + len;
		while(start < stop)
		{
			end = FindFirstOf(start, delimiters, delimCount);
			if(end == null)
			{
				end = stop;
			}
			if(end != start || includeEmpty)
			{
				tokens.push_back(container_t::value_type(start, end - start));
			}
			start = end + 1;
		}
	}

	//////////////////////////////////////////////////////////////////////
	// in place

	template <class T> void ToUpper(T &str)
	{
		for(auto & c : str)
		{
			c = toupper(c);
		}
	}

	//////////////////////////////////////////////////////////////////////
	// return a new one

	template <class T> T ToUpper(T const &str)
	{
		T t(str);
		ToUpper(t);
		return t;
	}

	//////////////////////////////////////////////////////////////////////

	struct RegKeyTraits
	{
		static void Close(HKEY key)			{	RegCloseKey(key);				}
		static HKEY InvalidValue()			{	return (HKEY)null;				}
	};

	//////////////////////////////////////////////////////////////////////

	struct HandleTraits
	{
		static void Close(HANDLE handle)	{	CloseHandle(handle);			}
		static HANDLE InvalidValue()		{	return INVALID_HANDLE_VALUE;	}
	};

	//////////////////////////////////////////////////////////////////////

	struct IUnknownTraits
	{
		static void Close(IUnknown *comptr)	{	comptr->Release();				}
		static IUnknown *InvalidValue()		{	return DX::null;				}
	};

	//////////////////////////////////////////////////////////////////////

	template<typename T, typename traits> struct ObjHandle
	{
		T obj;

		ObjHandle(T p) : obj(p)
		{
		}

		ObjHandle() : obj(traits::InvalidValue())
		{
		}

		operator T()
		{
			return obj;
		}

		T &operator = (T p)
		{
			obj = p;
			return obj;
		}

		T *operator &()
		{
			return &obj;
		}

		bool IsValid() const
		{
			return obj != traits::InvalidValue();
		}

		void Close()
		{
			if(IsValid())
			{
				traits::Close(obj);
			}
			obj = traits::InvalidValue();
		}

		void Release()
		{
			Close();
		}

		~ObjHandle()
		{
			Close();
		}
	};

	//////////////////////////////////////////////////////////////////////

	template<typename T> using DXPtr2 = ObjHandle < T, IUnknownTraits > ;
	using Handle = ObjHandle < HANDLE, HandleTraits > ;
	using RegKey = ObjHandle < HKEY, RegKeyTraits > ;

	//////////////////////////////////////////////////////////////////////

	inline string AppendBackslash(string const &s)
	{
		string p(s);
		if(p.size() > 0 && p.back() != '\\')
		{
			p.append("\\");
		}
		return p;
	}

	//////////////////////////////////////////////////////////////////////

	inline float Deg2Rad(float rad)
	{
		return rad * (PI / 180.0f);
	}

	//////////////////////////////////////////////////////////////////////

	inline float Rad2Deg(float deg)
	{
		return deg * (180.0f / PI);
	}

	//////////////////////////////////////////////////////////////////////

	template<typename T> T IntPow(T base, T exp)
	{
		T r;
		for(r = 1; exp != 0; exp >>= 1, base *= base)
		{
			if(exp & 1)
			{
				r *= base;
			}
		}
		return r;
	}

	////////////////////////////////////////////////////////////////////////

	struct Rational: DXGI_RATIONAL
	{
		operator float() const
		{
			return (float)Numerator / Denominator;
		}
	};

	////////////////////////////////////////////////////////////////////////

	Vec4f GetPlane(CVec4f p1, CVec4f p2, CVec4f p3);
}