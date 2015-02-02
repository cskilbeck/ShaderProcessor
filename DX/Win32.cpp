//////////////////////////////////////////////////////////////////////

#include "DX.h"
#include <strsafe.h>

//////////////////////////////////////////////////////////////////////

tstring Win32ErrorMessage(DWORD err)
{
	LPTSTR lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				  NULL,
				  err ? err : GetLastError(),
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				  (LPTSTR)&lpMsgBuf,
				  0, NULL);

	tstring r(lpMsgBuf);
	LocalFree(lpMsgBuf);
	return r;
}

//////////////////////////////////////////////////////////////////////

long ErrorMsgBox(tchar const *msg, long error)
{
	MessageBox(null, Format(TEXT("%s\r\n\r\n%08x:\r\n%s"), msg, error, Win32ErrorMessage(error).c_str()).c_str(), TEXT("Error"), MB_ICONEXCLAMATION);
	return error;
}

//////////////////////////////////////////////////////////////////////

long ErrorMsgBox(tchar const *msg)
{
	long error = GetLastError();
	MessageBox(null, Format(TEXT("%s\r\n\r\n%08x:\r\n%s"), msg, error, Win32ErrorMessage(error).c_str()).c_str(), TEXT("Error"), MB_ICONEXCLAMATION);
	return error;
}

//////////////////////////////////////////////////////////////////////

void ChangeDirectory(tchar const *relative)
{
	vector<tchar> s;
	s.resize(GetCurrentDirectory(0, null));
	GetCurrentDirectory((DWORD)s.size(), s.data());
	s[s.size() - 1] = tchar('\\');
	for(; *relative; ++relative)
	{
		s.push_back(*relative);
	}
	s.push_back(0);
	SetCurrentDirectory(s.data());
}

