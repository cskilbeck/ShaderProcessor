//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	typedef uint8_t			uint8;
	typedef uint16_t		uint16;
	typedef uint32_t		uint32;
	typedef uint64_t		uint64;
	typedef int8_t			int8;
	typedef int16_t			int16;
	typedef int32_t			int32;
	typedef int64_t			int64;
	typedef unsigned int	uint;
	typedef wchar_t			wchar;
	typedef TCHAR			tchar;
	typedef intptr_t		intptr;
	typedef uintptr_t		uintptr;

	const nullptr_t null = nullptr;

	template<typename T> using vector = std::vector < T > ;
	template<typename T> using aligned_vector = std::vector < T, aligned_allocator <T, 16> > ;
	using string = std::string;
	using wstring = std::wstring;
	using tstring = std::basic_string < tchar > ;

	template <typename T> using Ptr = std::unique_ptr < T > ;
}

#define WEAKSYM __declspec(selectany)

