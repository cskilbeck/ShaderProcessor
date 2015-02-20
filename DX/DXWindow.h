//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct DXWindow: Window
	{
		DXWindow(int width, int height, tchar const *caption = TEXT("DXWindow"), DX::DepthBufferOption depthBufferOption = DX::DepthBufferDisabled, DX::FullScreenOption fullScreenOption = DX::Windowed);
		virtual ~DXWindow();

		void Clear(DX::Color color);
		void ClearDepth(DepthClearOption option, float z = 1.0f, byte stencil = 0);

		void ResetRenderTargetView();

		virtual void OnDraw();

		void OnPaint(PAINTSTRUCT &ps) override;
		bool OnUpdate() override;
		bool OnCreate() override;
		void OnResized() override;
		void OnDestroy() override;
		void OnActivate() override;
		void OnDeactivate() override;
		void OnEnterSizeLoop() override;
		void OnExitSizeLoop() override;

		void Present();

		ID3D11Device *Device()
		{
			return mDevice.mDevice;
		}

		ID3D11DeviceContext *Context()
		{
			return mDevice.mContext;
		}

		D3DDevice	mDevice;
		HWND		mDXWindow;
		int			mFrame;
		Timer		mTimer;
	};

}