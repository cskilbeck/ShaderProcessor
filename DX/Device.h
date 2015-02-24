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
			DX::Context = mContext.get();

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
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		void ReleaseRenderTargets()
		{
			mContext->OMSetRenderTargets(0, null, null);
			mBackBuffer.Release();
			mDepthBuffer.Release();
			mDepthStencilView.Release();
			mRenderTargetView.Release();
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
			ReleaseRenderTargets();

			mContext->OMSetBlendState(null, null, 0);
			mContext->OMSetDepthStencilState(null, 0);
			mContext->RSSetState(null);

			if(mContext != null)
			{
				DXI(mContext->ClearState());
				DXI(mContext->Flush());
			}
			mSwapChain.Release();
			if(mContext != null)
			{
				DXI(mContext->ClearState());
				DXI(mContext->Flush());
			}
			mContext.Release();

#if defined(_DEBUG)
			DXPtr<ID3D11Debug> D3DDebug;
			DX::Device->QueryInterface(__uuidof(ID3D11Debug), (void **)&D3DDebug);
#endif

			mDevice.Release();
			mContext.Release();
			DX::Device = null;
			DX::Context = null;

#if defined(_DEBUG)
			D3DDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
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
		DXPtr<ID3D11DepthStencilView>	mDepthStencilView;
		DXPtr<ID3D11Texture2D>			mDepthBuffer;
	};
}