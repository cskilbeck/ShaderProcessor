//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

using namespace DX;

static void CenterRectInMonitor(Rect2D &rc, HMONITOR hMonitor)
{
	if(hMonitor != INVALID_HANDLE_VALUE)
	{
		MONITORINFO monitorInfo = { 0 };
		monitorInfo.cbSize = sizeof(monitorInfo);
		if(GetMonitorInfo(hMonitor, &monitorInfo))
		{
			int iMidX = (monitorInfo.rcWork.left + monitorInfo.rcWork.right) / 2;
			int iMidY = (monitorInfo.rcWork.top + monitorInfo.rcWork.bottom) / 2;
			int iRectWidth = rc.right - rc.left;
			int iRectHeight = rc.bottom - rc.top;
			rc.left = iMidX - iRectWidth / 2;
			rc.top = iMidY - iRectHeight / 2;
			rc.right = rc.left + iRectWidth;
			rc.bottom = rc.top + iRectHeight;
		}
	}
}

//////////////////////////////////////////////////////////////////////

static void CentreRectInMonitorWithMouseInIt(Rect2D &rc)
{
	Point2D ptCursorPos;
	GetCursorPos(&ptCursorPos);
	CenterRectInMonitor(rc, MonitorFromPoint(ptCursorPos, MONITOR_DEFAULTTOPRIMARY));
}

//////////////////////////////////////////////////////////////////////

static void CentreRectInDefaultMonitor(Rect2D &rc)
{
	CenterRectInMonitor(rc, MonitorFromPoint(Point2D(0, 0), MONITOR_DEFAULTTOPRIMARY));
}

//////////////////////////////////////////////////////////////////////

namespace DX
{
	int Window::sWindowClassIndex = 0;

	Window::Window(int width, int height, tchar const *caption, uint32 windowStyle, tchar const *className, HWND parent)
		: mHWND(null)
		, mHINST(null)
		, mActive(false)
		, mResizing(false)
		, mCaption(caption == null ? tstring() : caption)
		, mClassName(className == null ? tstring() : className)
		, mWindowStyle(windowStyle)
		, mParentHWND(parent)
		, mClosed(false)
		, mCreated(false)
	{
		mWindowInfo.cbSize = sizeof(WINDOWINFO);

		mWindowInfo.rcWindow = mWindowInfo.rcClient = Rect2D(0, 0, width, height);
		AdjustWindowRectEx(&mWindowInfo.rcWindow, windowStyle, false, 0);
	}

	//////////////////////////////////////////////////////////////////////

	bool Window::Open()
	{
		if(!Init(WindowWidth(), WindowHeight()))
		{
			Close();
			return false;
		}
		Show();
		return true;
	}

	//////////////////////////////////////////////////////////////////////

	void Window::Center()
	{
		Rect2D rc = ClientRect();
		CenterRectInMonitor(rc, MonitorFromWindow(mHWND, MONITOR_DEFAULTTOPRIMARY));
		AdjustWindowRectEx(&rc, GetStyle(), HasMenu(), GetExStyle());
		SetWindowPos(mHWND, null, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOSIZE | SWP_NOZORDER);
	}

	//////////////////////////////////////////////////////////////////////

	void AttachTo(HWND w)
	{
		// Subclass the window
	}

	//////////////////////////////////////////////////////////////////////

	bool Window::Init(int width, int height)
	{
		mHINST = GetModuleHandle(null);

		if(mClassName.empty())
		{
			mClassName = Format("WindowClass%d", sWindowClassIndex++);
			WNDCLASSEX wcex;
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wcex.lpfnWndProc = WndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = sizeof(intptr);
			wcex.hInstance = 0;
			wcex.hIcon = null;
			wcex.hCursor = LoadCursor(null, IDC_ARROW);
			wcex.hbrBackground = null;
			wcex.lpszMenuName = null;
			wcex.lpszClassName = mClassName.c_str();
			wcex.hIconSm = null;
			RegisterClassEx(&wcex);
		}

		assert((mParentHWND != null) == ((mWindowStyle & WS_CHILD) != 0));

		mHWND = CreateWindowEx(0, mClassName.c_str(), mCaption.c_str(), mWindowStyle, CW_USEDEFAULT, 0, width, height, mParentHWND, null, mHINST, this);
		if(mHWND == null)
		{
			DWORD err = GetLastError();
			ErrorMessageBox(TEXT("Failed to create window (%08x) - %s"), err, Win32ErrorMessage(err).c_str());
			return false;
		}
		GetWindowInfo(mHWND, &mWindowInfo);
		return true;
	}

	//////////////////////////////////////////////////////////////////////

	void Window::SetWindowRect(Rect2D const &r)
	{
		SetWindowPos(mHWND, null, r.left, r.top, r.Width(), r.Height(), SWP_NOZORDER);
	}

	//////////////////////////////////////////////////////////////////////

	long Window::GetStyle() const
	{
		return mWindowInfo.dwStyle;
	}

	//////////////////////////////////////////////////////////////////////

	long Window::GetExStyle() const
	{
		return mWindowInfo.dwExStyle;
	}

	//////////////////////////////////////////////////////////////////////

	bool Window::HasMenu() const
	{
		return GetMenu(mHWND) != null;
	}

	//////////////////////////////////////////////////////////////////////

	bool Window::IsResizable() const
	{
		return (GetStyle() & WS_THICKFRAME) != 0;
	}

	//////////////////////////////////////////////////////////////////////

	Rect2D Window::GetBorders() const
	{
		return Rect2D(0, 0, mWindowInfo.cxWindowBorders, mWindowInfo.cyWindowBorders);
	}

	//////////////////////////////////////////////////////////////////////

	Rect2D Window::GetWindowRectFromClientRect(Rect2D const &clientRect)
	{
		Rect2D rc = GetBorders();
		int minSize = IsResizable() ? GetSystemMetrics(SM_CXMINTRACK) : 0;
		rc.left += clientRect.left;
		rc.top += clientRect.top;
		rc.right += clientRect.right;
		rc.bottom += clientRect.bottom;
		rc.right = rc.left + Max(rc.Width(), minSize);
		return rc;
	}

	//////////////////////////////////////////////////////////////////////

	Rect2D Window::GetClientRectFromWindowRect(Rect2D const &windowRect)
	{
		Rect2D rc = GetBorders();
		Point2D tl(windowRect.TopLeft() - rc.TopLeft());
		Point2D br(windowRect.BottomRight() - rc.BottomRight());
		return Rect2D(tl, Size2D(br - tl));
	}

	//////////////////////////////////////////////////////////////////////

	void Window::SetClientSize(int newWidth, int newHeight)
	{
		Rect2D oldRect;
		Rect2D rc = GetWindowRectFromClientRect(Rect2D(0, 0, newWidth, newHeight));
		GetWindowRect(mHWND, &oldRect);
		rc.MoveTo(oldRect.MidPoint() - rc.HalfSize());
		rc.left = Max(0L, rc.left);
		rc.top = Max(0L, rc.top);
		SetWindowRect(rc);
	}

	//////////////////////////////////////////////////////////////////////

	void Window::SetWindowSize(int newWidth, int newHeight)
	{
		Rect2D rc(0, 0, newWidth, newHeight);
		AdjustWindowRectEx(&rc, GetStyle(), HasMenu(), GetExStyle());
		SetWindowPos(mHWND, null, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}

	//////////////////////////////////////////////////////////////////////

	void Window::MoveTo(int x, int y)
	{
		SetWindowPos(mHWND, null, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	//////////////////////////////////////////////////////////////////////

	bool Window::Update()
	{
		if(!mHWND && !Open() && !mCreated)
		{
			TRACE("Window::Update() - false!\n");
			return false;
		}

		Keyboard::LastKeyPressed = 0;
		Keyboard::LastCharPressed = 0;

		Mouse::Pressed = 0;
		Mouse::Released = 0;

		if(mHWND == null || mActive == false && WaitMessage() == 0)
		{
			TRACE("Window::Update()\\WaitMessage() - false!\n");
			return false;
		}

		MSG msg;
		while(PeekMessage(&msg, null, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT)
		{
			TRACE("Window::Update()\\WM_QUIT\n");
			return false;
		}

		Mouse::Update(*this);
		Keyboard::Update();

		if(mActive && !mResizing)
		{
			CallOnUpdate();
		}
		else
		{
			TRACE("CallOnUpdate NOT called\n");
		}
		return !mClosed;
	}

	//////////////////////////////////////////////////////////////////////

	void Window::CallOnUpdate()
	{
		if(mCreated && !mClosed)
		{
			OnUpdate();
		}
	}

	//////////////////////////////////////////////////////////////////////
	// The 1st WM_GETMINMAXINFO message will not get routed

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		Window *w;

#if 1
		if(GetKeyState(VK_CAPITAL) & 1)
		{
			TRACE("%08x: %s %08x,%08x\n", msg, GetWM_Name(msg), wParam, lParam);
		}
#endif

		if(msg == WM_NCCREATE)
		{
			w = (Window *)((CREATESTRUCT *)lParam)->lpCreateParams;
			SetWindowLongPtr(hWnd, 0, (LONG_PTR)w);
			w->mHWND = hWnd;
		}
		else
		{
			w = (Window *)GetWindowLongPtr(hWnd, 0);
		}

		return w ? w->HandleMessage(hWnd, msg, wParam, lParam) : DefWindowProc(hWnd, msg, wParam, lParam);
	}

	//////////////////////////////////////////////////////////////////////

	LRESULT Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		MINMAXINFO *mminfo;
		PAINTSTRUCT ps;
		MousePos mousePos;
		Point2D mouseScreenPos;

		switch(msg)
		{
			case WM_GETMINMAXINFO:
				mminfo = (MINMAXINFO *)lParam;
				mminfo->ptMinTrackSize.y = 16 + GetSystemMetrics(SM_CYMINTRACK) + (HasMenu() ? GetSystemMetrics(SM_CYMENU) : 0);
				break;

			case WM_CREATE:
				if(!OnCreate())
				{
					Close();
				}
				else
				{
					Created.Invoke(WindowEvent(this));
					mCreated = true;
				}
				break;

			case WM_DESTROY:
				Destroying.Invoke(WindowEvent(this));
				OnDestroy();
				Destroyed.Invoke(WindowEvent(this));
				PostQuitMessage(0);
				break;

			case WM_ERASEBKGND:
				return true;

			case WM_SIZE:
				DoResize();
				break;

			case WM_MOVE:
				DoMove();
				break;

			case WM_SIZING:
				DoResize();
				break;

			case WM_MOUSEWHEEL:
				Mouse::WheelDelta = (float)(HIWORD(wParam)) / WHEEL_DELTA;
				mouseScreenPos = GetPoint2DFromParam(lParam);
				ScreenToClient(hWnd, &mouseScreenPos);
				mousePos.x = (int16)mouseScreenPos.x;
				mousePos.y = (int16)mouseScreenPos.y;
				OnMouseWheel(mousePos, (int16)HIWORD(wParam), wParam);
				break;

			case WM_PAINT:
				BeginPaint(mHWND, &ps);
				OnPaint(ps);
				EndPaint(mHWND, &ps);
				break;

			case WM_ENTERSIZEMOVE:
				mResizing = true;
				OnEnterSizeLoop();
				break;

			case WM_EXITSIZEMOVE:
				mResizing = false;
				OnExitSizeLoop();
				break;

			case WM_CHAR:
				Keyboard::LastCharPressed = (int)wParam;
				OnChar((int)wParam, lParam);
				break;

			case WM_KEYDOWN:
				Keyboard::LastKeyPressed = (int)wParam;
				OnKeyDown((int)wParam, lParam);
				KeyPressed.Invoke(KeyboardEvent(this, (int)wParam));
				break;

			case WM_KEYUP:
				OnKeyUp((int)wParam, lParam);
				KeyReleased.Invoke(KeyboardEvent(this, (int)wParam));
				break;

			case WM_MOUSEMOVE:
				mousePos = GetMousePosFromParam(lParam);
				OnMouseMove(mousePos, wParam);
				MouseMoved.Invoke(MouseEvent(this, mousePos));
				break;

			case WM_LBUTTONDBLCLK:
				OnLeftMouseDoubleClick(GetMousePosFromParam(lParam));
				MouseDoubleClicked.Invoke(MouseButtonEvent(this, mousePos, MouseButtonEvent::Left));
				break;

			case WM_RBUTTONDBLCLK:
				OnRightMouseDoubleClick(GetMousePosFromParam(lParam));
				MouseDoubleClicked.Invoke(MouseButtonEvent(this, mousePos, MouseButtonEvent::Right));
				break;

			case WM_LBUTTONDOWN:
				Mouse::Pressed |= Mouse::Button::Left;
				Mouse::Held |= Mouse::Button::Left;
				OnLeftButtonDown(GetMousePosFromParam(lParam), wParam);
				MouseButtonPressed.Invoke(MouseButtonEvent(this, mousePos, MouseButtonEvent::Left));
				break;

			case WM_LBUTTONUP:
				Mouse::Released |= Mouse::Button::Left;
				Mouse::Held &= ~Mouse::Button::Left;
				OnLeftButtonUp(GetMousePosFromParam(lParam), wParam);
				MouseButtonReleased.Invoke(MouseButtonEvent(this, mousePos, MouseButtonEvent::Left));
				break;

			case WM_RBUTTONDOWN:
				Mouse::Pressed |= Mouse::Button::Right;
				Mouse::Held |= Mouse::Button::Right;
				OnRightButtonDown(GetMousePosFromParam(lParam), wParam);
				MouseButtonPressed.Invoke(MouseButtonEvent(this, mousePos, MouseButtonEvent::Right));
				break;

			case WM_RBUTTONUP:
				Mouse::Released |= Mouse::Button::Right;
				Mouse::Held &= ~Mouse::Button::Right;
				OnRightButtonUp(GetMousePosFromParam(lParam), wParam);
				MouseButtonReleased.Invoke(MouseButtonEvent(this, mousePos, MouseButtonEvent::Right));
				break;

			case WM_ACTIVATEAPP:
				SetActive(wParam == TRUE);
				break;

			case WM_ACTIVATE:
				SetActive(wParam != WA_INACTIVE);
				break;

			case WM_CLOSE:
				Close();
				break;

			case WM_NCMOUSEMOVE:
				OnNCMouseMove(GetMousePosFromParam(lParam), wParam);
				return DefWindowProc(hWnd, msg, wParam, lParam);

			case WM_NCLBUTTONDOWN:
				OnNCLButtonDown(GetMousePosFromParam(lParam), wParam);
				return DefWindowProc(hWnd, msg, wParam, lParam);

			case WM_NCLBUTTONUP:
				OnNCLButtonUp(GetMousePosFromParam(lParam), wParam);
				return DefWindowProc(hWnd, msg, wParam, lParam);

			case WM_WINDOWPOSCHANGED:
				OnWindowPosChanged((WINDOWPOS *)lParam);
				return DefWindowProc(hWnd, msg, wParam, lParam);

			case WM_WINDOWPOSCHANGING:
				OnWindowPosChanging((WINDOWPOS *)lParam);
				break;

			default:
				return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return 0;
	}

	//////////////////////////////////////////////////////////////////////

	bool Window::IsActive() const
	{
		return mActive;
	}

	//////////////////////////////////////////////////////////////////////

	void Window::SetActive(bool active)
	{
		mActive = active;
		Mouse::OnActivate(active, *this);
		if(mActive)
		{
			OnActivate();
		}
		else
		{
			OnDeactivate();
		}
		Activated.Invoke(WindowActivationEvent(this, mActive));
	}

	//////////////////////////////////////////////////////////////////////

	void Window::DoResize()
	{
		Rect2D r;
		GetClientRect(mHWND, &r);
		TRACE("DoResize(): %d,%d\n", r.Width(), r.Height());
		GetWindowInfo(mHWND, &mWindowInfo);
		TRACE("DoResize()2: %d,%d\n", ClientWidth(), ClientHeight());
		OnResized();
		Resized.Invoke(WindowSizedEvent(this, (Rect2D &)mWindowInfo.rcWindow, Size2D(ClientWidth(), ClientHeight())));
	}

	//////////////////////////////////////////////////////////////////////

	void Window::DoMove()
	{
		GetWindowInfo(mHWND, &mWindowInfo);
		OnMoved();
	}

	//////////////////////////////////////////////////////////////////////

	Window::~Window()
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::Show()
	{
		ShowWindow(mHWND, SW_SHOW);
	}

	//////////////////////////////////////////////////////////////////////

	void Window::Hide()
	{
		ShowWindow(mHWND, SW_HIDE);
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnPaint(PAINTSTRUCT &ps)
	{
		FillRect(ps.hdc, &ClientRect(), (HBRUSH)GetStockObject(WHITE_BRUSH));
		tchar hello[] = TEXT("Override your OnPaint function, fool!");
		SetTextAlign(ps.hdc, TA_CENTER | VTA_CENTER);
		TextOut(ps.hdc, ClientWidth() / 2, ClientHeight() / 2, hello, _countof(hello) - 1);
	}

	//////////////////////////////////////////////////////////////////////

	bool Window::OnCreate()
	{
		return true;
	}

	//////////////////////////////////////////////////////////////////////

	bool Window::OnUpdate()
	{
		return true;
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnResized()
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnMoved()
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnDestroy()
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnMouseMove(MousePos pos, uintptr flags)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnLeftMouseDoubleClick(MousePos pos)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnRightMouseDoubleClick(MousePos pos)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnLeftButtonDown(MousePos pos, uintptr flags)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnNCLButtonDown(MousePos pos, uintptr hitTestValue)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnNCLButtonUp(MousePos pos, uintptr hitTestValue)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnLeftButtonUp(MousePos pos, uintptr flags)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnRightButtonDown(MousePos pos, uintptr flags)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnRightButtonUp(MousePos pos, uintptr flags)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnChar(int key, uintptr flags)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnKeyDown(int key, uintptr flags)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnKeyUp(int key, uintptr flags)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnMouseWheel(MousePos pos, int delta, uintptr flags)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnActivate()
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnDeactivate()
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnEnterSizeLoop()
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnExitSizeLoop()
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnNCMouseMove(MousePos pos, uintptr hitTestValue)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnWindowPosChanged(WINDOWPOS *pos)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::OnWindowPosChanging(WINDOWPOS *pos)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Window::Close()
	{
		mClosed = true;
		DestroyWindow(mHWND);
	}

}