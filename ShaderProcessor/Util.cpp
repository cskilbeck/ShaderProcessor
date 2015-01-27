//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <time.h>

//////////////////////////////////////////////////////////////////////

void Trace(wchar const *strMsg, ...)
{
	wchar strBuffer[512];
	va_list args;
	va_start(args, strMsg);
	_vsnwprintf_s(strBuffer, ARRAYSIZE(strBuffer), strMsg, args);
	va_end(args);
	OutputDebugStringW(strBuffer);
}

//////////////////////////////////////////////////////////////////////

void Trace(char const *strMsg, ...)
{
	char strBuffer[512];
	va_list args;
	va_start(args, strMsg);
	_vsnprintf_s(strBuffer, ARRAYSIZE(strBuffer), strMsg, args);
	va_end(args);
	OutputDebugStringA(strBuffer);
}

//////////////////////////////////////////////////////////////////////

HRESULT LoadResource(uint32 resourceid, void **data, size_t *size)
{
	HRSRC myResource = ::FindResource(NULL, MAKEINTRESOURCE(resourceid), RT_RCDATA);
	if(myResource == null)
	{
		return ERROR_RESOURCE_DATA_NOT_FOUND;
	}

	HGLOBAL myResourceData = ::LoadResource(NULL, myResource);
	if(myResourceData == null)
	{
		return ERROR_RESOURCE_FAILED;
	}

	void *pMyBinaryData = ::LockResource(myResourceData);
	if(pMyBinaryData == null)
	{
		return ERROR_RESOURCE_NOT_AVAILABLE;
	}

	size_t s = (size_t)SizeofResource(GetModuleHandle(null), myResource);
	if(s == 0)
	{
		return ERROR_RESOURCE_FAILED;
	}

	if(size != null)
	{
		*size = s;
	}
	if(data != null)
	{
		*data = pMyBinaryData;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

uint8 *LoadFile(tchar const *filename, size_t *size)
{
	uint8 *buf = null;
	FILE *f = null;

	if(_tfopen_s(&f, filename, TEXT("rb")) == 0)
	{
		fseek(f, 0, SEEK_END);
		uint32 len = ftell(f);
		fseek(f, 0, SEEK_SET);

		buf = new uint8[len + sizeof(tchar)];

		if(buf != null)
		{
			size_t s = fread_s(buf, len, 1, len, f);

			if(s != len)
			{
				delete [] buf;
				buf = null;
			}
			else
			{
				*((tchar *)(((uint8 *)buf) + len)) = (tchar)'\0';
				if(size != null)
				{
					*size = len;
				}
			}
		}

		fclose(f);
	}
	else
	{
		MessageBox(null, Format(TEXT("File not found: %s"), filename).c_str(), TEXT("LoadFile"), MB_ICONERROR);
	}
	return buf;
}

//////////////////////////////////////////////////////////////////////

wstring Format(wchar const *fmt, ...)
{
	wchar buffer[1024];
	va_list v;
	va_start(v, fmt);
	vswprintf_s(buffer, fmt, v);
	return wstring(buffer);
}

//////////////////////////////////////////////////////////////////////

string Format(char const *fmt, ...)
{
	char buffer[1024];
	va_list v;
	va_start(v, fmt);
	_vsnprintf_s(buffer, sizeof(buffer), fmt, v);
	return string(buffer);
}

//////////////////////////////////////////////////////////////////////

wstring WideStringFromTString(tstring const &str)
{
	#ifdef UNICODE
		return str;
	#else
		return WideStringFromString(str);
	#endif
}

//////////////////////////////////////////////////////////////////////

wstring WideStringFromString(string const &str)
{
	vector<wchar> temp;
	temp.resize(str.size() + 1);
	temp[0] = (wchar)0;
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), &temp[0], (int)str.size());
	temp[str.size()] = 0;
	return wstring(&temp[0]);
}

//////////////////////////////////////////////////////////////////////

string StringFromWideString(wstring const &str)
{
	vector<char> temp;
	int bufSize = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.size() + 1, NULL, 0, NULL, FALSE);
	if(bufSize > 0)
	{
		temp.resize(bufSize);
		WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.size() + 1, &temp[0], bufSize, NULL, FALSE);
		return string(&temp[0]);
	}
	return string();
}

//////////////////////////////////////////////////////////////////////

string StringFromTString(tstring const &str)
{
#if defined(UNICODE)
	return StringFromWideString(str);
#else
	return str;
#endif
}

//////////////////////////////////////////////////////////////////////

tstring TStringFromString(string const &str)
{
#if defined(UNICODE)
	return WideStringFromString(str);
#else
	return str;
#endif
}

//////////////////////////////////////////////////////////////////////

tstring GetCurrentFolder()
{
	vector<tchar> s;
	DWORD l = GetCurrentDirectory(0, NULL);
	assert(l != 0);
	s.resize((size_t)l + 1);
	GetCurrentDirectory(l, &s[0]);
	s[l] = 0;
	return Format(TEXT("%s"), &s[0]);
}

//////////////////////////////////////////////////////////////////////

tstring GetDrive(tchar const *path)
{
	tchar Drive[MAX_PATH];
	tchar Dir[MAX_PATH];
	tchar Name[MAX_PATH];
	tchar Ext[MAX_PATH];
	_tsplitpath_s(path, Drive, Dir, Name, Ext);
	return Drive;
}

//////////////////////////////////////////////////////////////////////

tstring GetPath(tchar const *path)
{
	tchar Drive[MAX_PATH];
	tchar Dir[MAX_PATH];
	tchar Name[MAX_PATH];
	tchar Ext[MAX_PATH];
	_tsplitpath_s(path, Drive, Dir, Name, Ext);
	return Dir;
}

//////////////////////////////////////////////////////////////////////

tstring GetFilename(tchar const *path)
{
	tchar Drive[MAX_PATH];
	tchar Dir[MAX_PATH];
	tchar Name[MAX_PATH];
	tchar Ext[MAX_PATH];
	_tsplitpath_s(path, Drive, Dir, Name, Ext);
	return Name;
}

//////////////////////////////////////////////////////////////////////

tstring GetExtension(tchar const *path)
{
	tchar Drive[MAX_PATH];
	tchar Dir[MAX_PATH];
	tchar Name[MAX_PATH];
	tchar Ext[MAX_PATH];
	_tsplitpath_s(path, Drive, Dir, Name, Ext);
	return Ext;
}

//////////////////////////////////////////////////////////////////////

tstring SetExtension(tchar const *path, tchar const *ext)
{
	tchar Drive[MAX_PATH];
	tchar Dir[MAX_PATH];
	tchar Name[MAX_PATH];
	tchar Ext[MAX_PATH];
	_tsplitpath_s(path, Drive, Dir, Name, Ext);
	return tstring(Drive) + tstring(Dir) + tstring(Name) + tstring(ext);
}
