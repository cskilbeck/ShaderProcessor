//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct DXWindow: Window
{
	DXWindow(int width, int height, tchar const *caption = TEXT("DXWindow"), DepthBufferOption depthBufferOption = DepthBufferDisabled, FullScreenOption fullScreenOption = Windowed);
	virtual ~DXWindow();

	void Clear(Color color);

	virtual void OnDraw();

	void OnPaint(PAINTSTRUCT &ps) override;
	bool OnUpdate() override;
	bool OnCreate() override;
	void OnResized() override;
	void OnDestroy() override;

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
};

