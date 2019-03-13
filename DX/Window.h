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
		uint32	flags;
		wchar	key;

		KeyboardEvent(Window *w, uint32 flags, wchar key)
			: WindowEvent(w)
			, flags(flags)
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

	struct MouseWheelEvent: MouseEvent
	{
		float wheelDelta;

		MouseWheelEvent(Window *w, MousePos pos, float delta)
			: MouseEvent(w, pos)
			, wheelDelta(delta)
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
		Event<KeyboardEvent> SysKeyPressed;
		Event<KeyboardEvent> SysKeyReleased;
		Event<MouseEvent> MouseMoved;
		Event<MouseButtonEvent> MouseButtonPressed;
		Event<MouseButtonEvent> MouseButtonReleased;
		Event<MouseWheelEvent> MouseWheeled;
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
		virtual void OnSysKeyDown(int key, uintptr flags);
		virtual void OnSysKeyUp(int key, uintptr flags);
		virtual void OnKeyDown(int key, uintptr flags);
		virtual void OnKeyUp(int key, uintptr flags);
		virtual void OnActivate();
		virtual void OnDeactivate();
		virtual void OnEnterSizeLoop();
		virtual void OnExitSizeLoop();
		virtual void OnNCLButtonDown(MousePos pos, uintptr hitTestValue);
		virtual void OnNCLButtonUp(MousePos pos, uintptr hitTestValue);
		virtual void OnWindowPosChanging(WINDOWPOS *pos);
		virtual void OnWindowPosChanged(WINDOWPOS *pos);
		virtual void OnNCMouseMove(MousePos pos, uintptr hitTestValue);
        virtual void OnNCHitTest(MousePos pos, uintptr *hitTestValue);

		bool Open();
		void Close();

		bool Update();

		void Show();
		void Hide();

		void SetActive(bool active);
		bool IsActive() const;

		void Center();

		Size2D WindowSize() const;
		int WindowWidth() const;
		int WindowHeight() const;

		Size2D ClientSize() const;
		int ClientWidth() const;
		int ClientHeight() const;

		float FClientWidth() const;
		float FClientHeight() const;

		float FWindowWidth() const;
		float FWindowHeight() const;

		Vec2f FWindowSize() const;
		Vec2f FClientSize() const;

		Rect2D const &WindowRect() const;
		Rect2D const &ClientRect() const;

		bool SetMessageWait(bool wait);
		bool GetMessageWait() const;
		void MoveTo(int x, int y);
		void SetWindowRect(Rect2D const &r);
		void SetWindowSize(int newWidth, int newHeight);
		void SetClientSize(int newWidth, int newHeight);
		Rect2D GetWindowRectFromClientRect(Rect2D const &clientRect);
		Rect2D GetClientRectFromWindowRect(Rect2D const &windowRect);

		HWND Handle() const;

	protected:

		void CallOnUpdate();
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

		WINDOWINFO	mWindowInfo;
		HWND		mHWND;
		HWND		mParentHWND;
		HINSTANCE	mHINST;
		bool		mActive;
		bool		mResizing;
		bool		mMessageWait;
		tstring		mCaption;
		tstring		mClassName;
		uint32		mWindowStyle;
		bool		mClosed;
		bool		mCreated;

		static int	sWindowClassIndex;
	};

	//////////////////////////////////////////////////////////////////////

	inline HWND Window::Handle() const
	{
		return mHWND;
	}

	//////////////////////////////////////////////////////////////////////

	inline Rect2D const &Window::ClientRect() const
	{
		return (Rect2D const &)mWindowInfo.rcClient;
	}

	//////////////////////////////////////////////////////////////////////

	inline Rect2D const &Window::WindowRect() const
	{
		return (Rect2D const &)mWindowInfo.rcWindow;
	}

	//////////////////////////////////////////////////////////////////////

	inline Size2D Window::WindowSize() const
	{
		return WindowRect().Size();
	}

	//////////////////////////////////////////////////////////////////////

	inline int Window::WindowWidth() const
	{
		return WindowRect().Width();
	}

	//////////////////////////////////////////////////////////////////////

	inline int Window::WindowHeight() const
	{
		return WindowRect().Height();
	}

	//////////////////////////////////////////////////////////////////////

	inline Size2D Window::ClientSize() const
	{
		return ClientRect().Size();
	}

	//////////////////////////////////////////////////////////////////////

	inline int Window::ClientWidth() const
	{
		return ClientSize().Width();
	}

	//////////////////////////////////////////////////////////////////////

	inline int Window::ClientHeight() const
	{
		return ClientSize().Height();
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec2f Window::FClientSize() const
	{
		return Vec2f(FClientWidth(), FClientHeight());
	}

	//////////////////////////////////////////////////////////////////////

	inline float  Window::FClientWidth() const
	{
		return (float)ClientWidth();
	}

	//////////////////////////////////////////////////////////////////////

	inline float Window::FClientHeight() const
	{
		return (float)ClientHeight();
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec2f Window::FWindowSize() const
	{
		return Vec2f(FWindowWidth(), FWindowHeight());
	}

	//////////////////////////////////////////////////////////////////////

	inline float Window::FWindowWidth() const
	{
		return (float)WindowWidth();
	}

	//////////////////////////////////////////////////////////////////////

	inline float Window::FWindowHeight() const
	{
		return (float)WindowHeight();
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
		return Point2D((int16)GET_X_LPARAM(param), (int16)GET_Y_LPARAM(param));
	}

	//////////////////////////////////////////////////////////////////////

	inline MousePos GetMousePosFromParam(uintptr param)
	{
		return MousePos((int16)GET_X_LPARAM(param), (int16)GET_Y_LPARAM(param));
	}

	//////////////////////////////////////////////////////////////////////

	char const *GetWM_Name(uint message);
}