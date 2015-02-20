//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Window;

	//////////////////////////////////////////////////////////////////////

	struct WindowEvent
	{
		Window *window;

		WindowEvent(Window *window) : window(window)
		{
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct WindowActivationEvent: WindowEvent
	{
		bool active;

		WindowActivationEvent(Window *window, bool active)
			: WindowEvent(window)
			, active(active)
		{
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct WindowSizedEvent: WindowEvent
	{
		Rect2D windowPosition;
		Size2D clientSize;

		WindowSizedEvent(Window *w, Rect2D const &pos, Size2D const &size)
			: WindowEvent(w)
			, windowPosition(pos)
			, clientSize(size)
		{
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct KeyboardEvent: WindowEvent
	{
		wchar	key;

		KeyboardEvent(Window *w, wchar key)
			: WindowEvent(w)
			, key(key)
		{
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct MouseEvent: WindowEvent
	{
		MousePos position;

		MouseEvent(Window *w, MousePos pos)
			: WindowEvent(w)
			, position(pos)
		{
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct MouseButtonEvent: MouseEvent
	{
		enum Buttons
		{
			Left = 1,
			Right = 2,
			Middle = 4
		};

		Buttons buttons;

		MouseButtonEvent(Window *w, MousePos pos, Buttons buttons)
			: MouseEvent(w, pos)
			, buttons(buttons)
		{
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct Window
	{
		Window(int width = 800, int height = 450, tchar const *caption = TEXT("Window"), uint32 windowStyle = WS_OVERLAPPEDWINDOW, tchar const *className = null, HWND parent = null);
		~Window();

		Event<WindowEvent> Created;
		Event<WindowActivationEvent> Activated;
		Event<WindowActivationEvent> Deactivated;
		Event<WindowEvent> Destroying;
		Event<WindowEvent> Destroyed;
		Event<KeyboardEvent> KeyPressed;
		Event<KeyboardEvent> KeyReleased;
		Event<MouseEvent> MouseMoved;
		Event<MouseButtonEvent> MouseButtonPressed;
		Event<MouseButtonEvent> MouseButtonReleased;
		Event<MouseButtonEvent> MouseDoubleClicked;
		Event<WindowSizedEvent> Resized;

		virtual bool OnCreate();
		virtual bool OnUpdate();
		virtual void OnPaint(PAINTSTRUCT &ps);
		virtual void OnDestroy();
		virtual void OnMoved();
		virtual void OnResized();
		virtual void OnMouseMove(MousePos pos, uintptr flags);
		virtual void OnMouseWheel(MousePos pos, int delta, uintptr flags);
		virtual void OnLeftMouseDoubleClick(MousePos pos);
		virtual void OnRightMouseDoubleClick(MousePos pos);
		virtual void OnLeftButtonDown(MousePos pos, uintptr flags);
		virtual void OnLeftButtonUp(MousePos pos, uintptr flags);
		virtual void OnRightButtonDown(MousePos pos, uintptr flags);
		virtual void OnRightButtonUp(MousePos pos, uintptr flags);
		virtual void OnChar(int key, uintptr flags);
		virtual void OnKeyDown(int key, uintptr flags);
		virtual void OnKeyUp(int key, uintptr flags);
		virtual void OnActivate();
		virtual void OnDeactivate();
		virtual void OnEnterSizeLoop();
		virtual void OnExitSizeLoop();

		void Open();
		void Close();

		bool Update();

		void Show();
		void Hide();

		void SetActive(bool active);
		bool IsActive() const;

		void Center();
		int Width() const;
		int Height() const;
		int ClientWidth() const;
		int ClientHeight() const;
		float FWidth() const;
		float FHeight() const;
		Vec2f FSize() const;
		Rect2D ClientRect() const;
		bool SetMessageWait(bool wait);
		bool GetMessageWait() const;
		void MoveTo(int x, int y);
		void SetWindowRect(Rect2D const &r);
		void SetWindowSize(int newWidth, int newHeight);
		void SetClientSize(int newWidth, int newHeight);
		Rect2D GetWindowRectFromClientRect(Rect2D const &clientRect);
		Rect2D GetClientRectFromWindowRect(Rect2D const &windowRect);

		HWND Handle() const
		{
			return mHWND;
		}

	protected:

		void DoResize();
		void DoMove();
		bool Init(int width, int height);
		long GetStyle() const;
		long GetExStyle() const;
		bool HasMenu() const;
		bool IsResizable() const;
		Rect2D GetBorders() const;
		LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		friend LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		int			mWidth;
		int			mHeight;
		int			mClientWidth;
		int			mClientHeight;
		HWND		mHWND;
		HWND		mParentHWND;
		HINSTANCE	mHINST;
		bool		mActive;
		bool		mResizing;
		bool		mMessageWait;
		tstring		mCaption;
		tstring		mClassName;
		uint32		mWindowStyle;

		static int	sWindowClassIndex;
	};

	//////////////////////////////////////////////////////////////////////

	inline int Window::Width() const
	{
		return mWidth;
	}

	//////////////////////////////////////////////////////////////////////

	inline int Window::Height() const
	{
		return mHeight;
	}

	//////////////////////////////////////////////////////////////////////

	inline int Window::ClientWidth() const
	{
		return mClientWidth;
	}

	//////////////////////////////////////////////////////////////////////

	inline int Window::ClientHeight() const
	{
		return mClientHeight;
	}

	//////////////////////////////////////////////////////////////////////

	inline float Window::FWidth() const
	{
		return (float)mWidth;
	}

	//////////////////////////////////////////////////////////////////////

	inline float Window::FHeight() const
	{
		return (float)mHeight;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec2f Window::FSize() const
	{
		return Vec2f(FWidth(), FHeight());
	}

	//////////////////////////////////////////////////////////////////////

	inline Rect2D Window::ClientRect() const
	{
		Rect2D r;
		GetClientRect(mHWND, &r);
		return r;
	}

	//////////////////////////////////////////////////////////////////////

	inline bool Window::SetMessageWait(bool wait)
	{
		mMessageWait = wait;
	}

	//////////////////////////////////////////////////////////////////////

	inline bool Window::GetMessageWait() const
	{
		return mMessageWait;
	}

	//////////////////////////////////////////////////////////////////////

	inline Point2D GetPoint2DFromParam(uintptr param)
	{
		return Point2D((int16)LOWORD(param), (int16)HIWORD(param));
	}

	//////////////////////////////////////////////////////////////////////

	inline MousePos GetMousePosFromParam(uintptr param)
	{
		return MousePos((int16)LOWORD(param), (int16)HIWORD(param));
	}

	//////////////////////////////////////////////////////////////////////

	char const *GetWM_Name(uint message);
}