//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////
	// Global D3D Device and Context

	extern ID3D11Device *Device;
	extern ID3D11DeviceContext *Context;

	//////////////////////////////////////////////////////////////////////

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

	//////////////////////////////////////////////////////////////////////
	// A DisplayMode

	struct Monitor;
	struct Adapter;

	struct DisplayMode: list_node
	{
		DXGI_MODE_DESC1		mDesc;
		Monitor *			mMonitor;

		DisplayMode(DXGI_MODE_DESC1 &desc, Monitor *monitor)
		{
			mMonitor = monitor;
			mDesc = desc;
		}

		int Width() const
		{
			return mDesc.Width;
		}

		int Height() const
		{
			return mDesc.Height;
		}

		float RefreshRate() const
		{
			return (Rational &)mDesc.RefreshRate;
		}

		uint ScalingMode() const
		{
			return (uint)mDesc.Scaling;
		}

		char const *ScalingModeName() const
		{
			static char const *scalingModeNames[] =
			{
				"Unspecified",
				"Centered",
				"Stretched"
			};
			return scalingModeNames[ScalingMode()];
		}

		bool operator < (DisplayMode const &o) const
		{
			// prefer order: Stretched, Unspecified, Centered
			static int scalingRequired[] =
			{
				1,	// unspecified (middle)
				2,	// centered (worst)
				0	// stretched (best)
			};

			return scalingRequired[ScalingMode()] < scalingRequired[o.ScalingMode()];
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct Monitor: list_node
	{
		// each refresh rate has a list of display modes (which have different scaling modes)
		using scalingModeMap = std::map < float, linked_list<DisplayMode>, std::greater<float> > ;

		// each resolution has a map of <refreshRate, DisplayMode>
		using resolutionMap = std::map < Size2D, scalingModeMap > ;

		// the actual monitor object
		DXPtr<IDXGIOutput1>		mOutput;

		// descriptor
		DXGI_OUTPUT_DESC		mDesc;

		// map into the modes
		resolutionMap			mModesMap;

		// actual modes
		vector<DXGI_MODE_DESC1>	mDisplayModes;

		// which adapter this monitor is connected to
		Adapter *				mAdapter;

		operator IDXGIOutput1 **() const
		{
			return (IDXGIOutput1 **)&mOutput;
		}

		Monitor()
		{
		}

		~Monitor()
		{
		}

		HRESULT Init(DXPtr<IDXGIOutput> &output, Adapter *adapter)
		{
			mAdapter = adapter;

			// got a monitor, initialize it
			DXR(output.QueryInterface(mOutput));
			output.Release();

			DXR(mOutput->GetDesc(&mDesc));

			// scan the list of modes it supports
			uint numModes;
			DXR(mOutput->GetDisplayModeList1(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_SCALING, &numModes, null));
			mDisplayModes.resize(numModes);
			DXR(mOutput->GetDisplayModeList1(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_SCALING, &numModes, mDisplayModes.data()));

			// put the modes into the modes map
			for(auto &mode : mDisplayModes)
			{
				float rate = (Rational &)mode.RefreshRate;
				mModesMap[Size2D(mode.Width, mode.Height)][rate].push_back(new DisplayMode(mode, this));
			}

			// for each resolution
			for(auto &resolution : mModesMap)
			{
				// for each refreshrate at that resolution
				for(auto &mode : resolution.second)
				{
					// sort the modes by scaling type preference
					mode.second.sort();
				}
			}

			return S_OK;
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct Adapter: list_node
	{
		DXPtr<IDXGIAdapter1>	mAdapter;
		DXGI_ADAPTER_DESC1		mDesc;
		linked_list<Monitor>	mMonitors;

		static linked_list<Adapter>	sAdapters;

		wchar const *Description() const
		{
			return mDesc.Description;
		}

		operator IDXGIAdapter1 **() const
		{
			return (IDXGIAdapter1 **)&mAdapter;
		}

		operator IDXGIAdapter1 *() const
		{
			return mAdapter.get();
		}

		operator IDXGIAdapter *() const
		{
			return mAdapter.get();
		}

		Adapter()
		{
		}

		static Adapter *CreateAdapter()
		{
			Adapter *a = new Adapter();
			sAdapters.push_back(a);
			return a;
		}

		Monitor *CreateMonitor()
		{
			Monitor *m = new Monitor();
			mMonitors.push_back(m);
			return m;
		}

		HRESULT Init(DXPtr<IDXGIAdapter1> &adapter)
		{
			// initialize a GPU
			mAdapter = adapter;
			DXR(mAdapter->GetDesc1(&mDesc));

			// scan the connected monitors
			DXR(GetMonitors());
			return S_OK;
		}

		HRESULT GetMonitors()
		{
			// enumerator the connected monitors
			int i = 0;
			bool complete = false;
			while(!complete)
			{
				DXPtr<IDXGIOutput> output;
				HRESULT hr = mAdapter->EnumOutputs(i, &output);
				switch(hr)
				{
					case S_OK:
					{
						// got one, initialize it
						Monitor *m = CreateMonitor();
						DXR(m->Init(output, this));
					}
					break;

					case DXGI_ERROR_NOT_FOUND:
					{
						complete = true;
					}
					break;

					default:
					{
						return hr;
					}
				}
				++i;
			}
			return S_OK;
		}

		static void ShowDisplayModes()
		{
			for(auto const &gpu : sAdapters)
			{
				TRACE(L"GPU: %s\n", gpu.mDesc.Description);
				for(auto const &monitor : gpu.mMonitors)
				{
					TRACE(L"  MONITOR:%s\n", monitor.mDesc.DeviceName);
					for(auto const &resolution : monitor.mModesMap)
					{
						TRACE("    RESOLUTION:%dx%d\n", resolution.first.cx, resolution.first.cy);
						for(auto const &refreshRate : resolution.second)
						{
							TRACE("      REFRESH RATE:%5.2fHz\n", refreshRate.first);
							for(auto const &displayMode : refreshRate.second)
							{
								TRACE("        SCALING MODE:%s\n", displayMode.ScalingModeName());
							}
						}
						TRACE("\n");
					}
					TRACE("\n");
				}
				TRACE("\n");
			}
		}

		static DisplayMode const *FindDisplayMode(Size2D const &size)
		{
			for(auto const &resolution : GetDefaultAdapter()->GetDefaultMonitor()->mModesMap)
			{
				if(!(resolution.first < size))
				{
					return resolution.second.begin()->second.c_head();
				}
			}
			return null;
		}

		Monitor const *GetDefaultMonitor() const
		{
			return mMonitors.c_head();
		}

		static Adapter *GetDefaultAdapter()
		{
			// first one is always the default
			return sAdapters.head();
		}

		static void Close()
		{
			sAdapters.delete_all();
		}

		~Adapter()
		{
			mMonitors.delete_all();
		}

		// enumerate the GPUs
		static HRESULT ScanGPUs(IDXGIFactory1 *factory)
		{
			int i = 0;
			bool complete = false;
			while(!complete)
			{
				DXPtr<IDXGIAdapter1> adapter;
				HRESULT hr = factory->EnumAdapters1(i, &adapter);
				switch(hr)
				{
					case S_OK:
					{
						// got one, initialize it
						Adapter *a = CreateAdapter();
						a->Init(adapter);
					}
					break;
					
					case DXGI_ERROR_NOT_FOUND:
					{
						complete = true;
					}
					break;

					default:
					{
						return hr;
					}
				}
				++i;
			}
			ShowDisplayModes();
			return S_OK;
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct D3DDevice
	{
		D3DDevice(CreateSwapChainOption createSwapChainOption = WithSwapChain, BackBufferCount backBufferCount = BackBufferCount(2), DepthBufferOption depthBufferOption = DepthBufferDisabled, FullScreenOption fullScreenOption = FullScreen)
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
			TRACE("Device::Open() %d,%d\n", mWidth, mHeight);

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
			IDXGIAdapter *adapter = null;
			DXPtr<IDXGIFactory1> pFactory;
			D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_REFERENCE;

			if(mCreateSwapChainOption == WithSwapChain)
			{
				DXR(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory));
				DXR(Adapter::ScanGPUs(pFactory));	// get all GPUs, Monitors and DisplayModes
				adapter = Adapter::GetDefaultAdapter()->mAdapter;
				driverType = D3D_DRIVER_TYPE_UNKNOWN;
			}

			DXR(D3D11CreateDevice(adapter, driverType, null, flags, levels, _countof(levels), D3D11_SDK_VERSION, &mDevice, &mFeatureLevel, &mContext));

			if(mCreateSwapChainOption == WithSwapChain)
			{
				DXGI_SWAP_CHAIN_DESC swapChainDesc;
				swapChainDesc.SampleDesc.Count = 1;
				swapChainDesc.SampleDesc.Quality = 0;
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount = mBackBufferCount;
				swapChainDesc.OutputWindow = mWindow;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
				swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


				DisplayMode const *winner = Adapter::FindDisplayMode(r.Size());

				// found a mode?
				if(winner == null)
				{
					// no, stick with windowed mode
					swapChainDesc.Windowed = TRUE;
					swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					swapChainDesc.BufferDesc.Width = 0;
					swapChainDesc.BufferDesc.Height = 0;
					swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
					swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
					swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
					swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				}
				else
				{
					// yes, allow fullscreen if requested
					swapChainDesc.Windowed = mFullScreenOption == Windowed;
					DXGI_MODE_DESC1 const &m = winner->mDesc;
					swapChainDesc.BufferDesc.Format = m.Format;
					swapChainDesc.BufferDesc.Height = m.Height;
					swapChainDesc.BufferDesc.Width = m.Width;
					swapChainDesc.BufferDesc.RefreshRate = m.RefreshRate;
					swapChainDesc.BufferDesc.Scaling = m.Scaling;
					swapChainDesc.BufferDesc.ScanlineOrdering = m.ScanlineOrdering;
				}

				DXR(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, null, flags, levels, _countof(levels), D3D11_SDK_VERSION, &mDevice, &mFeatureLevel, &mContext));
				DXR(pFactory->CreateSwapChain(mDevice, &swapChainDesc, &mSwapChain));

				// Disallow Alt-Enter to toggle fullscreen
				//IDXGIFactory1 *f;
				//DXR(mSwapChain->GetParent(__uuidof(IDXGIFactory1), (void**)&f));
				//DXR(f->MakeWindowAssociation(mWindow, DXGI_MWA_NO_WINDOW_CHANGES));

				DXR(pFactory->MakeWindowAssociation(mWindow, DXGI_MWA_NO_ALT_ENTER));
				DXR(CreateDepthBuffer());
				DXR(GetBackBuffer());
			}

			Device = mDevice.get();
			Context = mContext.get();

			auto i = D3DLevelNames.find(mFeatureLevel);
			if(i != D3DLevelNames.end())
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

		void ResetViewport()
		{
			Viewport(0, 0, (float)mWidth, (float)mHeight, 0, 1).Activate(mContext);
		}

		//////////////////////////////////////////////////////////////////////

		void ResetRenderTargetView()
		{
			DXI(mContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView));
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT GetBackBuffer()
		{
			DXR(mSwapChain->GetBuffer(0, __uuidof(mBackBuffer), (void **)&mBackBuffer));
			DXR(mDevice->CreateRenderTargetView(mBackBuffer, null, &mRenderTargetView));
			ResetViewport();
			ResetRenderTargetView();
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT ReleaseRenderTargets()
		{
			DXI(mContext->ClearState());
			DXI(mContext->Flush());
			DXI(mContext->OMSetRenderTargets(0, null, null));

			mBackBuffer.Release();
			mDepthBuffer.Release();
			mDepthStencilView.Release();
			mRenderTargetView.Release();
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT ToggleFullScreen()
		{
			BOOL isFullScreen;
			DXR(mSwapChain->GetFullscreenState(&isFullScreen, null));
			DXR(mSwapChain->SetFullscreenState(!isFullScreen, null));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Resize(uint width, uint height)
		{
			if(mContext != null)
			{
				DXR(ReleaseRenderTargets());
				if(mSwapChain != null)
				{
					BOOL isFullScreen;
					IDXGIOutput *output;
					DXR(mSwapChain->GetFullscreenState(&isFullScreen, &output));
					TRACE("D3D::Resize(%d,%d) (Fullscreen = %d)\n", width, height, isFullScreen);
					mWidth = width;
					mHeight = height;

					DXR(mSwapChain->ResizeBuffers(mBackBufferCount, mWidth, mHeight, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
					DXGI_SWAP_CHAIN_DESC desc;
					DXR(mSwapChain->GetDesc(&desc));
					TRACE("SwapChain resized to %dx%d\n", desc.BufferDesc.Width, desc.BufferDesc.Height);
					DXR(CreateDepthBuffer());
					DXR(GetBackBuffer());
				}

			}
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		void Close()
		{
			TRACE("Device::Close()\n");
			mSwapChain->SetFullscreenState(false, null);

			ReleaseRenderTargets();
			mSwapChain.Release();
			if(mContext != null)
			{
				DXI(mContext->ClearState());
				DXI(mContext->Flush());
			}
			mContext.Release();
			DX::Context = null;

#			if defined(_DEBUG)
			DXPtr<ID3D11Debug> D3DDebug;
			DX::Device->QueryInterface(__uuidof(ID3D11Debug), (void **)&D3DDebug);
#			endif

			Adapter::Close();

			mDevice.Release();
			DX::Device = null;

#			if defined(_DEBUG)
			D3DDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			D3DDebug.Release();
#			endif
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

		DXPtr<IDXGIOutput>				mOutputMonitor;

		DXPtr<IDXGISwapChain>			mSwapChain;

		DXPtr<ID3D11Texture2D>			mBackBuffer;
		DXPtr<ID3D11RenderTargetView>	mRenderTargetView;

		DXPtr<ID3D11Texture2D>			mDepthBuffer;
		DXPtr<ID3D11DepthStencilView>	mDepthStencilView;
	};
}