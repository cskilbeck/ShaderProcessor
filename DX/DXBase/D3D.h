//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{

	using Matrix = DirectX::XMMATRIX;

	extern HRESULT __hr;

	//////////////////////////////////////////////////////////////////////

	extern std::map<int, string> D3DLevelNames;

#if defined(_DEBUG)
	void SetDebugName(ID3D11DeviceChild *child, tchar const *name);
#else
#define SetDebugName if(false) {} else
#endif

	char const *GetDXGIFormatName(DXGI_FORMAT format);
	char const *GetDXGIScanLineOrderingName(DXGI_MODE_SCANLINE_ORDER scanlineOrder);
	char const *GetDXGIScalingModeName(DXGI_MODE_SCALING scalingMode);

	//////////////////////////////////////////////////////////////////////

	class HRException: std::exception
	{
	public:
		HRException(HRESULT h = S_OK, char const *msg = null) : exception(msg), hr(h)
		{
		}

		HRESULT hr;
	};

	//////////////////////////////////////////////////////////////////////

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
#define DXI(x) (x);
#endif

	//////////////////////////////////////////////////////////////////////
	// THIS IS NOT THREAD SAFE

	template<typename T> struct DXPtr
	{
		DXPtr(T *init = null) : p(init)
		{
		}

		DXPtr(DXPtr const &ptr) : p(ptr.p)
		{
			p->AddRef();
		}

		DXPtr(DXPtr &&ptr) : p(nullptr)
		{
			std::swap(p, ptr.p);
		}

		DXPtr &operator=(DXPtr const &ptr)
		{
			Release();
			p = ptr.p;
			p->AddRef();
			return *this;
		}

		DXPtr &operator=(DXPtr&& ptr)
		{
			std::swap(p, ptr.p);
			return *this;
		}

		~DXPtr()
		{
			Release();
		}

		bool IsNull() const
		{
			return p == null;
		}

		T **operator &()
		{
			return &p;
		}

		T *operator->() const
		{
			return p;
		}

		T &operator *() const
		{
			return *p;
		}

		operator T *()
		{
			return p;
		}

		bool operator == (nullptr_t) const
		{
			return p == null;
		}

		bool operator != (nullptr_t) const
		{
			return p != null;
		}

		T *get() const
		{
			return p;
		}

		T *Relinquish()
		{
			T *o = p;
			p = null;
			return o;
		}

		LONG Release()
		{
			LONG r = 0;
			if(p != nullptr)
			{
				r = p->Release();
				p = nullptr;
			}
			return r;
		}

		template <typename T> HRESULT QueryInterface(DXPtr<T> &ptr)
		{
			DXR(p->QueryInterface(__uuidof(T), (void **)&ptr));
			return S_OK;
		}

		T *p;
	};


}