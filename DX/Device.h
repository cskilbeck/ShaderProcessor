//////////////////////////////////////////////////////////////////////

#pragma once

namespace DX
{
	struct BackBufferCount
	{
		explicit BackBufferCount(uint count)
			: mCount(count)
		{
		}

		operator uint() const
		{
			return mCount;
		}

		uint mCount;
	};

	extern ID3D11Device *Device;
	extern ID3D11DeviceContext *Context;

	struct D3DDevice
	{
		//////////////////////////////////////////////////////////////////////

		D3DDevice(CreateSwapChainOption createSwapChainOption = WithSwapChain, BackBufferCount backBufferCount = BackBufferCount(2), DepthBufferOption depthBufferOption = DepthBufferDisabled, FullScreenOption fullScreenOption = Windowed)
			: mWindow(null)
			, mWidth(0)
			, mHeight(0)
			, mDepthBufferOption(depthBufferOption)
			, mFullScreenOption(fullScreenOption)
			, mCreateSwapChainOption(createSwapChainOption)
			, mBackBufferCount(backBufferCount)
		{
		}

		//////////////////////////////////////////////////////////////////////

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
				swapChainDesc.BufferCount = mBackBufferCount;
				swapChainDesc.OutputWindow = mWindow;
				swapChainDesc.Windowed = mFullScreenOption == Windowed;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
				swapChainDesc.Flags = 0;

				//DXR(D3D11CreateDeviceAndSwapChain(null,
				//	D3D_DRIVER_TYPE_HARDWARE,
				//	null,
				//	flags,
				//	levels,
				//	_countof(levels),
				//	D3D11_SDK_VERSION,
				//	&swapChainDesc,
				//	&mSwapChain,
				//	&mDevice,
				//	&mFeatureLevel,
				//	&mContext));

				// get a DXGIFactory
				DXPtr<IDXGIFactory1> pFactory;
				DXR(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory));

				// Get the default video card
				DXPtr<IDXGIAdapter1> adapter;
				DXR(pFactory->EnumAdapters1(0, &adapter));

				// have a look
				DXGI_ADAPTER_DESC1 desc;
				DXR(adapter->GetDesc1(&desc));
				TRACE(L"Default Adapter: %s\n Video RAM: %p\nSystem RAM: %p\nShared RAM: %p\n\n", desc.Description, desc.DedicatedVideoMemory, desc.DedicatedSystemMemory, desc.SharedSystemMemory);

				// Get the default output (monitor)
				DXPtr<IDXGIOutput> output;
				DXR(adapter->EnumOutputs(0, &output));

				// have a look
				DXGI_OUTPUT_DESC monitorDesc;
				DXR(output->GetDesc(&monitorDesc));
				TRACE(L"Default Monitor: %s\n", monitorDesc.DeviceName);
				Rect2D &r = (Rect2D &)monitorDesc.DesktopCoordinates;
				TRACE("%dx%d\n", r.Width(), r.Height());

				// get the list of display modes that the default monitor supports
				uint32 numModes;
				DXR(output->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, 0, &numModes, null));
				vector<DXGI_MODE_DESC> modes(numModes);
				DXR(output->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, 0, &numModes, modes.data()));

				// group them by resolution
				std::map<Size2D, std::list<DXGI_MODE_DESC *>> modeMap;
				for(auto &mode : modes)
				{
					modeMap[Size2D(mode.Width, mode.Height)].push_back(&mode);
				}

				// have a look
				for(auto &l : modeMap)
				{
					TRACE("Resolution: %dx%d\n", l.first.Width(), l.first.Height());
					for(auto &m : l.second)
					{
						TRACE(L"    %5.2fHz,  %s\n", (float)m->RefreshRate.Numerator / m->RefreshRate.Denominator, m->Scaling == DXGI_MODE_SCALING_CENTERED ? L"Centered" : L"Scaled");
					}
					TRACE("\n");
				}

				DXR(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, null, flags, levels, _countof(levels), D3D11_SDK_VERSION, &mDevice, &mFeatureLevel, &mContext));
				DXR(pFactory->CreateSwapChain(mDevice, &swapChainDesc, &mSwapChain));
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

			Device = mDevice.get();
			Context = mContext.get();

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
				DXR(mDevice->CreateDepthStencilView(mDepthBuffer, null, &mDepthStencilView));
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
			ResetRenderTargetView();
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT ReleaseRenderTargets()
		{
			DXI(mContext->ClearState());
			DXI(mContext->Flush());

			mContext->OMSetRenderTargets(0, null, null);
			mBackBuffer.Release();
			mDepthBuffer.Release();
			mDepthStencilView.Release();
			mRenderTargetView.Release();
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Resize(uint width, uint height)
		{
			mWidth = width;
			mHeight = height;
			if(mContext != null)
			{
				DXR(ReleaseRenderTargets());
				DXR(mSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));
				DXR(CreateDepthBuffer());
				DXR(GetBackBuffer());

			}
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		void Close()
		{
			ReleaseRenderTargets();
			mSwapChain.Release();
			if(mContext != null)
			{
				DXI(mContext->ClearState());
				DXI(mContext->Flush());
			}
			mContext.Release();
			DX::Context = null;

#if defined(_DEBUG)
			DXPtr<ID3D11Debug> D3DDebug;
			DX::Device->QueryInterface(__uuidof(ID3D11Debug), (void **)&D3DDebug);
#endif

			mDevice.Release();
			DX::Device = null;

#if defined(_DEBUG)
			D3DDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			D3DDebug.Release();
#endif
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
		uint							mBackBufferCount;
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

		DXPtr<ID3D11Texture2D>			mDepthBuffer;
		DXPtr<ID3D11DepthStencilView>	mDepthStencilView;
	};
}