//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

DXWindow::DXWindow(int width, int height, tchar const *caption, DepthBufferOption depthBufferOption, FullScreenOption fullScreenOption)
	: Window(width, height, caption, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_SIZEBOX | WS_BORDER)
	, mDevice(WithSwapChain, depthBufferOption, fullScreenOption)
	, mFrame(0)
	, mDXWindow(null)
{
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

void DXWindow::OnResized()
{
	mDevice.Resize();
	OnUpdate();
}

//////////////////////////////////////////////////////////////////////

bool DXWindow::OnUpdate()
{
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
	mDevice.mContext->ClearRenderTargetView(mDevice.mRenderTargetView, color.GetFloatsRGBA(rgba));
}

//////////////////////////////////////////////////////////////////////

void DXWindow::ClearDepth(DepthClearOption option, float z, byte stencil)
{
	if(mDevice.mDepthStencilView != null)
	{
		uint flags;
		switch(option)
		{
			case Depth:
				flags = D3D11_CLEAR_DEPTH;
				break;
			case Stencil:
				flags = D3D11_CLEAR_STENCIL;
				break;
			case Depth + Stencil:
				flags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
				break;
			default:
				flags = 0;
		}
		Context()->ClearDepthStencilView(mDevice.mDepthStencilView, flags , z, stencil);
	}
}
