//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{

	tstring Win32ErrorMessage(DWORD err = 0);
	void ErrorMessageBox(tchar const *format, ...);
	void ChangeDirectory(tchar const *relative);
	char const *GetWM_Name(uint message);

	//////////////////////////////////////////////////////////////////////

	inline Point2D GetPointFromParam(uintptr param)
	{
		return Point2D((int16)LOWORD(param), (int16)HIWORD(param));
	}

}