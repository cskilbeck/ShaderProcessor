//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

namespace DX
{
	DXWindow::DXWindow(int width, int height, tchar const *caption, DepthBufferOption depthBufferOption, FullScreenOption fullScreenOption)
		: Window(width, height, caption, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_SIZEBOX | WS_BORDER)
		, mDevice(WithSwapChain, depthBufferOption, fullScreenOption)
		, mFrame(0)
		, mDXWindow(null)
	{
		mTimer.Reset();
		mDXWindow = mHWND;
		mMessageWait = false;
	}

	//////////////////////////////////////////////////////////////////////

	bool DXWindow::OnCreate()
	{
		return Window::OnCreate() && SUCCEEDED(mDevice.Open(mHWND));
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::OnDestroy()
	{
		mDevice.Close();
	}

	//////////////////////////////////////////////////////////////////////

	DXWindow::~DXWindow()
	{
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::OnPaint(PAINTSTRUCT &ps)
	{
		// Don't call Window::OnPaint(ps);
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::OnDraw()
	{
		Clear(Color::CornflowerBlue);
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::ResetRenderTargetView()
	{
		mDevice.ResetRenderTargetView();
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::OnResized()
	{
		mDevice.Resize(mClientWidth, mClientHeight);
		OnUpdate();
	}

	//////////////////////////////////////////////////////////////////////

	bool DXWindow::OnUpdate()
	{
		mTimer.Update();
		OnDraw();
		Present();
		if(!mResizing)
		{
			++mFrame;
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::Present()
	{
		mDevice.mSwapChain->Present(1, 0);
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::Clear(Color color)
	{
		float rgba[4];
		Context()->ClearRenderTargetView(mDevice.mRenderTargetView, color.GetFloatsRGBA(rgba));
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::OnActivate()
	{
		mTimer.UnPause();
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::OnDeactivate()
	{
		mTimer.Pause();
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::OnEnterSizeLoop()
	{
		mTimer.Pause();
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::OnExitSizeLoop()
	{
		mTimer.UnPause();
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::ClearDepth(DepthClearOption option, float z, byte stencil)
	{
		if(mDevice.mDepthStencilView != null)
		{
			Context()->ClearDepthStencilView(mDevice.mDepthStencilView, option, z, stencil);
		}
	}

}