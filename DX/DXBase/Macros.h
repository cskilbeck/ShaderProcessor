//////////////////////////////////////////////////////////////////////

namespace DX
{
	extern int __hr;

	class HRException: std::exception
	{
	public:
		HRException(HRESULT h = S_OK, char const *msg = null) : exception(msg), hr(h)
		{
		}

		HRESULT hr;
	};

}

//#define DXTRACE TRACE
#define DXTRACE if (true) {} else

#if defined(_DEBUG)
#define DXI(x) 			\
	(x);				\
	DXTRACE(TEXT(#x) TEXT( " done\n"));

#define DXR(x) 			\
	__hr = (x);			\
	if(FAILED(__hr))	\
	{					\
		TRACE(TEXT(#x) TEXT(" failed: %08x\n"), __hr); \
		assert(false);	\
		return __hr;	\
	}					\
	else				\
	{					\
		DXTRACE(TEXT(#x) TEXT( " ok\n")); \
	}					\

#define EXR(x) 			\
	__hr = (x);			\
	if(__hr != ERROR_SUCCESS)	\
	{					\
		TRACE(TEXT(#x) TEXT(" failed: %08x\n"), __hr); \
		assert(false);	\
		return __hr;	\
	}					\
	else				\
	{					\
		DXTRACE(TEXT(#x) TEXT( " ok\n")); \
	}					\

#define SXR(x) 			\
	__hr = (x);			\
	if(__hr != ERROR_SUCCESS)	\
	{					\
		return __hr;	\
	}

#define DXV(x) 			\
	__hr = (x);			\
	if(FAILED(__hr))	\
	{					\
		TRACE(TEXT(#x) TEXT(" failed: %08x\n"), __hr); \
		assert(false);	\
		return;			\
	}					\
	else				\
	{					\
		DXTRACE(TEXT(#x) TEXT( " ok\n")); \
	}					\

#define DXZ(x) 			\
	__hr = (x);			\
	if(FAILED(__hr))	\
	{					\
		TRACE(TEXT(#x) TEXT(" failed: %08x\n"), __hr); \
		assert(false);	\
		return 0;		\
	}					\
	else				\
	{					\
		DXTRACE(TEXT(#x) TEXT( " ok\n")); \
	}					\

#define DXB(x) 			\
	__hr = (x);			\
	if(FAILED(__hr))	\
	{					\
		TRACE(TEXT(#x) TEXT(" failed: %08x\n"), __hr); \
		assert(false);	\
		return false;	\
	}					\
	else				\
	{					\
		DXTRACE(TEXT(#x) TEXT( " ok\n")); \
	}					\

#define DXT(x) 			\
	__hr = (x);			\
	if(FAILED(__hr))	\
	{					\
		TRACE(TEXT(#x) TEXT(" failed: %08x\n"), __hr); \
		assert(false);	\
		throw HRException(__hr, #x); \
	}					\
	else				\
	{					\
		DXTRACE(TEXT(#x) TEXT( " ok\n")); \
	}	
#else
#define DXR(x) __hr = (x); if(FAILED(__hr)) return __hr;
#define DXV(x) __hr = (x); if(FAILED(__hr)) return;
#define DXZ(x) __hr = (x); if(FAILED(__hr)) return 0;
#define DXB(x) __hr = (x); if(FAILED(__hr)) return false;
#define DXT(x) __hr = (x); if(FAILED(__hr)) throw HRException(__hr, #x);
#define SXR(x) __hr = (x); if(__hr != ERROR_SUCCESS) return __hr;
#define EXR(x) __hr = (x); if(__hr != ERROR_SUCCESS) return __hr;
#define DXI(x) (x);
#endif

