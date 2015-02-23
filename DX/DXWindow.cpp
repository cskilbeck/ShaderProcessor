//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

namespace DX
{
	DXWindow::DXWindow(int width, int height, tchar const *caption, DepthBufferOption depthBufferOption, FullScreenOption fullScreenOption, BackBufferCount backBufferCount)
		: Window(width, height, caption, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_SIZEBOX | WS_BORDER)
		, mD3D(WithSwapChain, backBufferCount, depthBufferOption, fullScreenOption)
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
		return Window::OnCreate() && SUCCEEDED(mD3D.Open(mHWND));
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::OnDestroy()
	{
		mD3D.Close();
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
		mD3D.ResetRenderTargetView();
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::OnResized()
	{
		mD3D.Resize(ClientWidth(), ClientHeight());
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
		mD3D.mSwapChain->Present(1, 0);
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::Clear(Color color)
	{
		float rgba[4];
		Context()->ClearRenderTargetView(mD3D.mRenderTargetView, color.GetFloatsRGBA(rgba));
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::OnWindowPosChanging(WINDOWPOS *pos)
	{
		mTimer.Pause();
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::OnWindowPosChanged(WINDOWPOS *pos)
	{
		if(!mResizing)
		{
			mTimer.UnPause();
		}
	}

	//////////////////////////////////////////////////////////////////////

	void DXWindow::OnNCMouseMove(MousePos pos, uintptr hitTestValue)
	{
		if(mTimer.Paused() && !mResizing)
		{
			mTimer.UnPause();
		}
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
		if(mD3D.mDepthStencilView != null)
		{
			Context()->ClearDepthStencilView(mD3D.mDepthStencilView, option, z, stencil);
		}
	}

}