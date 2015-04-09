//////////////////////////////////////////////////////////////////////

#include "DX.h"
#include "Shaders/Font.shader.h"

//////////////////////////////////////////////////////////////////////

using namespace DX;

namespace
{
	Ptr<Font> debugFont;
	DrawList debugDrawList;
	DXWindow *mainWindow;
	Vec2f cursorPos;
}

//////////////////////////////////////////////////////////////////////

namespace DX
{
	void debug_open(DXWindow *w)
	{
		assert(FontManager::IsOpen());
		debugFont.reset(FontManager::Load("debug"));
		mainWindow = w;
	}

	void debug_close()
	{
		debugFont.reset();
		mainWindow = null;
	}

	void debug_begin()
	{
		cursorPos = Vec2f::zero;
		debugFont->SetDrawList(debugDrawList);
		debugFont->Setup(mainWindow->Context(), mainWindow);
		debugFont->Begin();
	}

	void debug_end()
	{
		debugFont->End();
		debugDrawList.End();
		mainWindow->ResetRenderTargetView();
		debugDrawList.Execute();
	}

	void debug_text(char const *fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		string s = Format_V(fmt, v);
		debugFont->DrawString(s.c_str(), cursorPos);
	}

	void debug_text(wchar const *fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		string s = StringFromWideString(Format_V(fmt, v));
		debugFont->DrawString(s.c_str(), cursorPos);
	}

	void debug_text(int x, int y, char const *fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		string s = Format_V(fmt, v);
		debugFont->DrawString(s.c_str(), Vec2f((float)x, (float)y));
	}

	void debug_text(int x, int y, wchar const *fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		string s = StringFromWideString(Format_V(fmt, v));
		debugFont->DrawString(s.c_str(), Vec2f((float)x, (float)y));
	}
}