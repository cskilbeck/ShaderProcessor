//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

static POINT mouseSavedPos;
static Vec2f oldMousePos;
static Mouse::Mode mouseMode = Mouse::Mode::Free;
static bool mouseVisible = true;

//////////////////////////////////////////////////////////////////////

static void ShowMouse()
{
	if(!mouseVisible)
	{
		ShowCursor(true);
		mouseVisible = true;
	}
}

//////////////////////////////////////////////////////////////////////

static void HideMouse()
{
	if(mouseVisible)
	{
		ShowCursor(false);
		mouseVisible = false;
	}
}

//////////////////////////////////////////////////////////////////////

namespace Mouse
{
	//////////////////////////////////////////////////////////////////////

	Vec2f		Delta(0, 0);
	float		WheelDelta(0);
	Vec2f		Position(0, 0);
	uint32		Pressed(0);
	uint32		Held(0);
	uint32		Released(0);

	//////////////////////////////////////////////////////////////////////

	void Update(Window &w)
	{
		if(w.IsActive())
		{
			Point2D p;
			HideMouse();
			GetCursorPos(&p);
			ScreenToClient(w.Handle(), &p);
			if(GetMode() == Mode::Captured)
			{
				POINT c;
				c.x = w.Width() / 2;
				c.y = w.Height() / 2;
				Mouse::Delta = Vec2f((float)p.x - c.x, (float)p.y - c.y);
				Position += Mouse::Delta;
				ClientToScreen(w.Handle(), &c);
				SetCursorPos(c.x, c.y);
			}
		}
		else
		{
			ShowMouse();
		}
	}

	//////////////////////////////////////////////////////////////////////

	void SetMode(Mode mode, Window &w)
	{
		if(mouseMode != mode)
		{
			mouseMode = mode;
			if(mode == Captured)
			{
				GetCursorPos(&mouseSavedPos);
				HideMouse();
				oldMousePos = Mouse::Position;
				Mouse::Update(w);
				Mouse::Delta = Vec2f(0, 0);
			}
			else
			{
				SetCursorPos(mouseSavedPos.x, mouseSavedPos.y);
				Mouse::Position = oldMousePos;
				ShowMouse();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////

	void OnActivate(bool active, Window &w)
	{
		if(active)
		{
			if(GetMode() == Mode::Captured)
			{
				GetCursorPos(&mouseSavedPos);
				HideMouse();
				SetCapture(w.Handle());
				POINT p;
				p.x = w.Width() / 2;
				p.y = w.Height() / 2;
				ClientToScreen(w.Handle(), &p);
				SetCursorPos(p.x, p.y);
			}
		}
		else
		{
			ReleaseCapture();
			ShowMouse();
			if(GetMode() == Mode::Captured)
			{
				SetCursorPos(mouseSavedPos.x, mouseSavedPos.y);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////

	Mode GetMode()
	{
		return mouseMode;
	}

	//////////////////////////////////////////////////////////////////////
}
