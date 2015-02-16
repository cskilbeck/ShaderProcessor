//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Window;

	struct Event
	{
	};

	struct WindowEvent : Event
	{
		Window *window;

		WindowEvent(Window *w) : window(w)
		{
		}
	};

	struct KeyboardEvent: WindowEvent
	{
		enum Action : uint16
		{
			Pressed = 0,
			Released = 1
		};

		Action	action;
		wchar	key;

		KeyboardEvent(Window *w, Action action, wchar key)
			: WindowEvent(w)
			, action(action)
			, key(key)
		{
		}
	};

	struct MouseEvent: WindowEvent
	{
		Point2DS position;

		MouseEvent(Window *w, int x, int y)
			: WindowEvent(w)
			, position(x, y)
		{
		}
	};

	struct MouseButtonEvent: MouseEvent
	{
		enum Buttons: uint16
		{
			Left = 1,
			Right = 2,
			Middle = 4
		};

		enum Action: uint16
		{
			Pressed = 0,
			Released = 1
		};

		Buttons buttons;
		Action action;

		MouseButtonEvent(Window *w, int x, int y, Buttons buttons, Action action)
			: MouseEvent(w, x, y)
			, buttons(buttons)
			, action(action)
		{
		}
	};

	using EventFunction = std::function < void(Event &event) > ;

	struct EventHandler : list_node<EventHandler>
	{
		EventFunction handler;	// CRAP! Not templated...
	};

	struct EventList
	{
		linked_list<EventHandler> handlers;

		EventHandler *AddListener(EventFunction function)
		{
			EventHandler *e = new EventHandler();	// CRAP! Heap allocation...
			e->handler = function;
			handlers.push_back(e);
			return e;
		}

		void Remove(EventHandler *e)
		{
			handlers.remove(e);
			delete e;
		}

		void Fire(void *event)
		{
			for(auto &e : handlers)
			{
				e.handler(*(Event *)event);
			}
		}
	};

	struct Window
	{
		Window(int width = 640, int height = 480, tchar const *caption = TEXT("Window"), uint32 windowStyle = WS_OVERLAPPEDWINDOW, tchar const *className = null, HWND parent = null);
		~Window();

		EventList BeforeDestroy;
		EventList AfterDestroy;

		virtual bool OnCreate();
		virtual bool OnUpdate();
		virtual void OnPaint(PAINTSTRUCT &ps);
		virtual void OnDestroy();
		virtual void OnMoved();
		virtual void OnResized();
		virtual void OnMouseMove(Point2D pos, uintptr flags);
		virtual void OnMouseWheel(Point2D pos, int delta, uintptr flags);
		virtual void OnLeftMouseDoubleClick(Point2D pos);
		virtual void OnRightMouseDoubleClick(Point2D pos);
		virtual void OnLeftButtonDown(Point2D pos, uintptr flags);
		virtual void OnLeftButtonUp(Point2D pos, uintptr flags);
		virtual void OnRightButtonDown(Point2D pos, uintptr flags);
		virtual void OnRightButtonUp(Point2D pos, uintptr flags);
		virtual void OnChar(int key, uintptr flags);
		virtual void OnKeyDown(int key, uintptr flags);
		virtual void OnKeyUp(int key, uintptr flags);

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

	inline Point2D GetPointFromParam(uintptr param)
	{
		return Point2D((int16)LOWORD(param), (int16)HIWORD(param));
	}

	//////////////////////////////////////////////////////////////////////

	char const *GetWM_Name(uint message);
}