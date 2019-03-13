//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

namespace DX {
DXWindow::DXWindow(int width, int height, tchar const *caption, DepthBufferOption depthBufferOption, FullScreenOption fullScreenOption, BackBufferCount backBufferCount,
                   DXGI_SWAP_EFFECT swap_effect, WindowBorderOption window_border_option, int vsync_frames)
    : Window(width, height, caption, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | ((window_border_option == WithWindowBorder) ? (WS_OVERLAPPEDWINDOW | WS_SIZEBOX | WS_BORDER) : (WS_POPUP)))
    , mD3D(WithSwapChain, backBufferCount, depthBufferOption, fullScreenOption, swap_effect)
    , mFrame(0)
    , mDXWindow(null)
{
    mTimer.Reset();
    mDXWindow = mHWND;
    mMessageWait = false;
    mBaseAspectRatio = (float)width / height;

    // ALT-Enter toggles fullscreen...

    mSysKeyPressedDelegate = [this](KeyboardEvent const &e) {
        if(e.key == VK_RETURN) {
            mD3D.ToggleFullScreen();
        }
    };
    SysKeyPressed += mSysKeyPressedDelegate;
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

void DXWindow::OnFrame()
{
    Clear(Color::CornflowerBlue);
}

//////////////////////////////////////////////////////////////////////

void DXWindow::ResetRenderTargetView()
{
    mD3D.ResetRenderTargetView();
}

//////////////////////////////////////////////////////////////////////

void DXWindow::ResetViewport()
{
    mD3D.ResetViewport();
}

//////////////////////////////////////////////////////////////////////

void DXWindow::OnResized()
{
    if(mD3D.mContext != null) {
        mD3D.Resize(ClientWidth(), ClientHeight());
        CallOnUpdate();
        mMessageWait = false;
    }
}

//////////////////////////////////////////////////////////////////////

bool DXWindow::OnUpdate()
{
    mTimer.Update();
    OnFrame();
    Present();
    if(!mResizing) {
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
    if(!mResizing) {
        mTimer.UnPause();
    }
}

//////////////////////////////////////////////////////////////////////

void DXWindow::OnNCHitTest(MousePos pos, uintptr *hitTestValue)
{
    // hold alt key to drag window by the client area
    if((GetAsyncKeyState(VK_MENU) & 0x8000) != 0) {
        *hitTestValue = HTCAPTION;
    }
}

//////////////////////////////////////////////////////////////////////

void DXWindow::OnNCMouseMove(MousePos pos, uintptr hitTestValue)
{
    if(mTimer.Paused() && !mResizing) {
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
    if(mD3D.mDepthStencilView != null) {
        Context()->ClearDepthStencilView(mD3D.mDepthStencilView, option, z, stencil);
    }
}

}    // namespace DX