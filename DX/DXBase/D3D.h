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

	extern ID3D11Device *Device;

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

	//////////////////////////////////////////////////////////////////////

	struct D3DDevice
	{
		D3DDevice(CreateSwapChainOption createSwapChainOption = WithSwapChain, DepthBufferOption depthBufferOption = DepthBufferDisabled, FullScreenOption fullScreenOption = Windowed)
			: mWindow(null)
			, mWidth(0)
			, mHeight(0)
			, mDepthBufferOption(depthBufferOption)
			, mFullScreenOption(fullScreenOption)
			, mCreateSwapChainOption(createSwapChainOption)
		{
		}

		HRESULT Open(HWND w = null)
		{
			mWindow = w;
			Rect2D r;
			GetClientRect(w, &r);
			mWidth = r.Width();
			mHeight = r.Height();

			D3D_FEATURE_LEVEL levels[] =
			{
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3,
				D3D_FEATURE_LEVEL_9_2,
				D3D_FEATURE_LEVEL_9_1
			};

#if defined(_DEBUG)
			UINT flags = D3D11_CREATE_DEVICE_DEBUG;
#else
			UINT flags = 0;
#endif
			if(mCreateSwapChainOption == WithSwapChain)
			{
				DXGI_SWAP_CHAIN_DESC swapChainDesc;
				swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				swapChainDesc.BufferDesc.Width = 0;
				swapChainDesc.BufferDesc.Height = 0;
				swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
				swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
				swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				swapChainDesc.SampleDesc.Count = 1;
				swapChainDesc.SampleDesc.Quality = 0;
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount = 2;
				swapChainDesc.OutputWindow = mWindow;
				swapChainDesc.Windowed = mFullScreenOption == Windowed;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
				swapChainDesc.Flags = 0;

				DXR(D3D11CreateDeviceAndSwapChain(null,
					D3D_DRIVER_TYPE_HARDWARE,
					null,
					flags,
					levels,
					_countof(levels),
					D3D11_SDK_VERSION,
					&swapChainDesc,
					&mSwapChain,
					&mDevice,
					&mFeatureLevel,
					&mContext));

				DXR(CreateDepthBuffer());
				DXR(GetBackBuffer());
			}
			else
			{
				DXR(D3D11CreateDevice(null,
					D3D_DRIVER_TYPE_REFERENCE,
					null,
					flags,
					levels,
					_countof(levels),
					D3D11_SDK_VERSION,
					&mDevice,
					&mFeatureLevel,
					&mContext));
			}

			DX::Device = mDevice.get();

			auto i = DX::D3DLevelNames.find(mFeatureLevel);
			if(i != DX::D3DLevelNames.end())
			{
				DXTRACE("%s Device created\n", i->second.c_str());
			}
			else
			{
				DXTRACE("Unknown D3D feature level!?\n");
			}
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT CreateDepthBuffer()
		{
			if(mDepthBufferOption == DepthBufferEnabled)
			{
				mDepthBuffer.Release();

				D3D11_TEXTURE2D_DESC depth;
				depth.Width = mWidth;
				depth.Height = mHeight;
				depth.MipLevels = 0;
				depth.ArraySize = 1;
				depth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				depth.SampleDesc.Count = 1;
				depth.SampleDesc.Quality = 0;
				depth.Usage = D3D11_USAGE_DEFAULT;
				depth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				depth.CPUAccessFlags = 0;
				depth.MiscFlags = 0;
				DXR(mDevice->CreateTexture2D(&depth, null, &mDepthBuffer));
			}
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		void ResetRenderTargetView()
		{
			D3D11_VIEWPORT vp;
			vp.TopLeftX = 0.0f;
			vp.TopLeftY = 0.0f;
			vp.Width = (float)mWidth;
			vp.Height = (float)mHeight;
			vp.MaxDepth = 1.0f;
			vp.MinDepth = 0.0f;
			DXI(mContext->RSSetViewports(1, &vp));
			DXI(mContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView));
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT GetBackBuffer()
		{
			DXR(mSwapChain->GetBuffer(0, __uuidof(mBackBuffer), (void **)&mBackBuffer));
			DXR(mDevice->CreateRenderTargetView(mBackBuffer, null, &mRenderTargetView));
			if(mDepthBuffer != null)
			{
				CreateDepthBuffer();
				DXR(mDevice->CreateDepthStencilView(mDepthBuffer, null, &mDepthStencilView));
			}
			ResetRenderTargetView();

			// default viewport for now
			D3D11_VIEWPORT vp;
			vp.TopLeftX = 0.0f;
			vp.TopLeftY = 0.0f;
			vp.Width = (float)mWidth;
			vp.Height = (float)mHeight;
			vp.MaxDepth = 1.0f;
			vp.MinDepth = 0.0f;
			DXI(mContext->RSSetViewports(1, &vp));

			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Resize(uint width, uint height)
		{
			mWidth = width;
			mHeight = height;
			if(mContext != null)
			{
				DXI(mContext->ClearState());
				DXI(mContext->Flush());
				DXI(mContext->OMSetRenderTargets(0, null, null));
				mBackBuffer.Release();
				mRenderTargetView.Release();
				mDepthStencilView.Release();
				DXR(mSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));
				DXR(GetBackBuffer());
			}
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		void Close()
		{
			if(mContext != null)
			{
				DXI(mContext->ClearState());
				DXI(mContext->Flush());
			}
			mRenderTargetView.Release();
			mDepthStencilView.Release();
			mBackBuffer.Release();
			mDepthBuffer.Release();
			mSwapChain.Release();
			mContext.Release();

#if defined(_DEBUG)
			if(DX::Device != null)
			{
				DXPtr<ID3D11Debug> D3DDebug;
				DX::Device->QueryInterface(__uuidof(ID3D11Debug), (void **)&D3DDebug);
				D3DDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
				D3DDebug.Release();
			}
#endif

			mDevice.Release();
			DX::Device = null;
			mFeatureLevel = (D3D_FEATURE_LEVEL)0;
		}

		//////////////////////////////////////////////////////////////////////

		~D3DDevice()
		{
			Close();
		}

		//////////////////////////////////////////////////////////////////////

		int								mWidth;
		int								mHeight;
		HWND							mWindow;
		CreateSwapChainOption			mCreateSwapChainOption;
		DepthBufferOption				mDepthBufferOption;
		FullScreenOption				mFullScreenOption;
		D3D_FEATURE_LEVEL				mFeatureLevel;

		DXPtr<ID3D11Device>				mDevice;
		DXPtr<ID3D11DeviceContext>		mContext;
		DXPtr<IDXGISwapChain>			mSwapChain;
		DXPtr<ID3D11Texture2D>			mBackBuffer;
		DXPtr<ID3D11RenderTargetView>	mRenderTargetView;
		DXPtr<ID3D11DepthStencilView>	mDepthStencilView;
		DXPtr<ID3D11Texture2D>			mDepthBuffer;
	};

}