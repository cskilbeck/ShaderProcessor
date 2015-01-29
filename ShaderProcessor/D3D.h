//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

using Matrix = DirectX::XMMATRIX;

extern HRESULT __hr;

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

#define DXTRACE TRACE
//#define DXTRACE if (true) {} else

#if defined(DEBUG)
#define DX(x) 			\
	__hr = (x);			\
	if(FAILED(__hr))	\
	{					\
		TRACE(TEXT(#x) TEXT(" failed: %08x\n"), __hr);	\
		assert(false);	\
		return __hr;	\
	}					\
	else				\
	{					\
		DXTRACE(TEXT(#x) TEXT( " ok\n"));		\
	}					\

#define DXV(x) 			\
	__hr = (x);			\
	if(FAILED(__hr))	\
	{					\
		TRACE(TEXT(#x) TEXT(" failed: %08x\n"), __hr);	\
		assert(false);	\
		return;			\
	}					\
	else				\
	{					\
		DXTRACE(TEXT(#x) TEXT( " ok\n"));		\
	}					\

#define DXB(x) 			\
	__hr = (x);			\
	if(FAILED(__hr))	\
	{					\
		TRACE(TEXT(#x) TEXT(" failed: %08x\n"), __hr);	\
		assert(false);	\
		return false;	\
	}					\
	else				\
	{					\
		DXTRACE(TEXT(#x) TEXT( " ok\n"));		\
	}					\

#define DXT(x) 			\
	__hr = (x);			\
	if(FAILED(__hr))	\
	{					\
		TRACE(TEXT(#x) TEXT(" failed: %08x\n"), __hr);	\
		assert(false);	\
		throw HRException(__hr, #x);	\
	}					\
	else				\
	{					\
		DXTRACE(TEXT(#x) TEXT( " ok\n"));		\
	}	
#else
#define DX(x) __hr = (x); if(FAILED(__hr)) return __hr;
#define DXV(x) __hr = (x); if(FAILED(__hr)) return;
#define DXB(x) __hr = (x); if(FAILED(__hr)) return false;
#define DXT(x) __hr = (x); if(FAILED(__hr)) throw HRException(__hr, #x);
#endif

//////////////////////////////////////////////////////////////////////

template<typename T> struct HandleTraits
{
	static void Close(T &handle) { }
	static T Default() { }
	static T InvalidValue() { }
};

//////////////////////////////////////////////////////////////////////

template<> struct HandleTraits<HANDLE>
{
	static void Close(HANDLE handle)
	{
		CloseHandle(handle);
	}

	static HANDLE Default()
	{
		return INVALID_HANDLE_VALUE;
	}

	static HANDLE InvalidValue()
	{
		return INVALID_HANDLE_VALUE;
	}
};

//////////////////////////////////////////////////////////////////////

template<> struct HandleTraits<IUnknown *>
{
	static void Close(IUnknown *comptr)
	{
		if(comptr != null)
		{
			comptr->Release();
		}
	}

	static IUnknown *Default()
	{
		return null;
	}

	static IUnknown *InvalidValue()
	{
		return null;
	}
};

//////////////////////////////////////////////////////////////////////

template<typename T, typename traits> struct ObjHandle
{
	T obj;

	ObjHandle(T p) : obj(p)
	{
	}

	ObjHandle()
	{
		obj = traits::Default();
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

	~ObjHandle()
	{
		if(obj != traits::InvalidValue())
		{
			traits::Close(obj);
		}
		obj = traits::Default();
	}
};

//////////////////////////////////////////////////////////////////////

template<typename T> using DXPtr2 = ObjHandle<T, HandleTraits<IUnknown *>>;
using Handle = ObjHandle<HANDLE, HandleTraits<HANDLE>>;

//////////////////////////////////////////////////////////////////////

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

	T *p;
};

//////////////////////////////////////////////////////////////////////

extern DXPtr<ID3D11Device> gDevice;
extern DXPtr<ID3D11DeviceContext> gContext;

//////////////////////////////////////////////////////////////////////

namespace D3D
{
	bool Open();
	void Close();

	struct Initializer
	{
		Initializer()
		{
			IsValid = Open();
		}

		~Initializer()
		{
			Close();
		}
		bool IsValid;
	};

#if defined(_DEBUG)
	void SetDebugName(ID3D11DeviceChild *child, tchar const *name);
#else
#define SetDebugName if(false) {} else
#endif

}

