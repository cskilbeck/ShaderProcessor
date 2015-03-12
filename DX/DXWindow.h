//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct DXWindow: Window
	{
		DXWindow(int width, int height,
				 tchar const *caption = TEXT("DXWindow"),
				 DepthBufferOption depthBufferOption = DX::DepthBufferDisabled,
				 FullScreenOption fullScreenOption = DX::Windowed,
				 BackBufferCount backBufferCount = BackBufferCount(2));

		virtual ~DXWindow();

		void Clear(DX::Color color);
		void ClearDepth(DepthClearOption option, float z = 1.0f, byte stencil = 0);

		void ResetRenderTargetView();

		virtual void OnFrame();

		void OnPaint(PAINTSTRUCT &ps) override;
		bool OnUpdate() override;
		bool OnCreate() override;
		void OnResized() override;
		void OnDestroy() override;
		void OnActivate() override;
		void OnDeactivate() override;
		void OnEnterSizeLoop() override;
		void OnExitSizeLoop() override;
		void OnWindowPosChanging(WINDOWPOS *pos) override;
		void OnWindowPosChanged(WINDOWPOS *pos) override;
		void OnNCMouseMove(MousePos pos, uintptr hitTestValue) override;

		void Present();

		ID3D11Device *Device()
		{
			return mD3D.mDevice;
		}

		ID3D11DeviceContext *Context()
		{
			return mD3D.mContext;
		}

		D3DDevice	mD3D;
		HWND		mDXWindow;
		int			mFrame;
		Timer		mTimer;
	};

}