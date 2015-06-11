//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

using namespace DX;

static POINT mouseSavedPos;
static Vec2f oldMousePos;
static Mouse::Mode mouseMode = Mouse::Mode::Free;

//////////////////////////////////////////////////////////////////////

static bool IsMouseVisible()
{
	CURSORINFO ci;
	ci.cbSize = sizeof(ci);
	GetCursorInfo(&ci);
	return ci.flags == CURSOR_SHOWING;
}

//////////////////////////////////////////////////////////////////////

static void ShowMouse()
{
	if(!IsMouseVisible())
	{
		ShowCursor(true);
	}
}

//////////////////////////////////////////////////////////////////////

static void HideMouse()
{
	if(IsMouseVisible())
	{
		ShowCursor(false);
	}
}

//////////////////////////////////////////////////////////////////////

namespace DX
{
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
			Point2D p;
			GetCursorPos(&p);
			ScreenToClient(w.Handle(), &p);

			if(w.IsActive() && GetMode() == Mode::Captured)
			{
				HideMouse();
				Point2D c(w.ClientRect().HalfSize());
				Mouse::Delta = Vec2f((float)p.x - c.x, (float)p.y - c.y);
				Position += Mouse::Delta;
				ClientToScreen(w.Handle(), &c);
				SetCursorPos(c.x, c.y);
			}
			else
			{
				Position = p;
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
					Point2D p = (Point2D)w.ClientSize() / 2;
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

}