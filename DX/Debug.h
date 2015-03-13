//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct DXWindow;

	void debug_open(DXWindow *w);
	void debug_close();

	void debug_begin();
	void debug_end();

	void debug_text(char const *fmt, ...);
	void debug_text(wchar const *fmt, ...);
	void debug_text(int x, int y, char const *fmt, ...);
	void debug_text(int x, int y, wchar const *fmt, ...);
}
