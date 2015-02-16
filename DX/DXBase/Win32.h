//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	tstring Win32ErrorMessage(DWORD err = 0);
	long ErrorMsgBox(tchar const *format);
	long ErrorMsgBox(tchar const *format, long error);
	void ChangeDirectory(tchar const *relative);
}