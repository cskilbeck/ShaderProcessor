//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

static byte kbPressed[256];
static byte kbReleased[256];
static byte kbHeld[256];
static byte kbPrevious[256];

//////////////////////////////////////////////////////////////////////

namespace DX
{

	namespace Keyboard
	{
		//////////////////////////////////////////////////////////////////////

		int LastKeyPressed;
		int LastCharPressed;
		int LastKeyReleased;

		//////////////////////////////////////////////////////////////////////

		void Reset()
		{
			memset(kbPressed, 0, sizeof(kbPressed));
			memset(kbHeld, 0, sizeof(kbHeld));
			memset(kbReleased, 0, sizeof(kbReleased));
			LastCharPressed = 0;
			LastKeyPressed = 0;
		}

		//////////////////////////////////////////////////////////////////////

		bool Pressed(uint keyCode)
		{
			return (kbPressed[keyCode] & 0x80) != 0;
		}

		//////////////////////////////////////////////////////////////////////

		bool Held(uint keyCode)
		{
			return (kbHeld[keyCode] & 0x80) != 0;
		}

		//////////////////////////////////////////////////////////////////////

		bool Released(uint keyCode)
		{
			return (kbReleased[keyCode] & 0x80) != 0;
		}

		//////////////////////////////////////////////////////////////////////

		void Update()
		{
			GetKeyboardState(kbHeld);
			for(uint i = 0; i < 256; ++i)
			{
				byte delta = kbHeld[i] ^ kbPrevious[i];
				kbPressed[i] = delta & kbHeld[i];
				kbReleased[i] = delta & ~kbHeld[i];
				kbPrevious[i] = kbHeld[i];
			}
		}
	}

}