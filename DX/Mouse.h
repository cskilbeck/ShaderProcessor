//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct Window;

namespace Mouse
{
	//////////////////////////////////////////////////////////////////////

	extern Vec2f	Delta;
	extern float	WheelDelta;
	extern Vec2f	Position;
	extern uint32	Pressed;
	extern uint32	Held;
	extern uint32	Released;

	//////////////////////////////////////////////////////////////////////

	enum Mode
	{
		Captured,
		Free
	};

	//////////////////////////////////////////////////////////////////////

	enum Button
	{
		Left = 1,
		Right = 2
	};

	//////////////////////////////////////////////////////////////////////

	void Update(Window &w);
	void SetMode(Mode mode, Window &w);
	Mode GetMode();
	void OnActivate(bool active, Window &w);

	//////////////////////////////////////////////////////////////////////

}