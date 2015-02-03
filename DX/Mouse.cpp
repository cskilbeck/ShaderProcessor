//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

static POINT mouseSavedPos;
static Vec2f oldMousePos;
static Mouse::Mode mouseMode = Mouse::Mode::Free;

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
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(w.Handle(), &p);
		Position = Vec2f((float)p.x, (float)p.y);
		if(GetMode() == Mode::Captured)
		{
			POINT c;
			c.x = w.Width() / 2;
			c.y = w.Height() / 2;
			Mouse::Delta = Vec2f((float)p.x - c.x, (float)p.y - c.y);
			ClientToScreen(w.Handle(), &c);
			SetCursorPos(c.x, c.y);
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
				ShowCursor(false);
				GetCursorPos(&mouseSavedPos);
				oldMousePos = Mouse::Position;
				Mouse::Update(w);
				Mouse::Delta = Vec2f(0, 0);
			}
			else
			{
				SetCursorPos(mouseSavedPos.x, mouseSavedPos.y);
				Mouse::Position = oldMousePos;
				ShowCursor(true);
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
