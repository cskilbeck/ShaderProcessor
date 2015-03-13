//////////////////////////////////////////////////////////////////////

#include "DXBase.h"
#include <time.h>
#include <debugapi.h>

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	string Format_V(char const *fmt, va_list v)
	{
		char buffer[512];
		int l = _vsnprintf_s(buffer, _countof(buffer), _TRUNCATE, fmt, v);
		if(l != -1)
		{
			return string(buffer);
		}
		l = _vscprintf(fmt, v);
		char *buf = new char[l + 1];
		l = _vsnprintf_s(buf, l + 1, _TRUNCATE, fmt, v);
		string s(buf, buf + l);
		delete(buf);
		return s;
	}

	//////////////////////////////////////////////////////////////////////

	wstring Format_V(wchar const *fmt, va_list v)
	{
		wchar buffer[512];
		int l = _vsnwprintf_s(buffer, _countof(buffer), _TRUNCATE, fmt, v);
		if(l != -1)
		{
			return wstring(buffer);
		}
		l = _vscwprintf(fmt, v);
		wchar *buf = new wchar[l + 1];
		l = _vsnwprintf_s(buf, l + 1, _TRUNCATE, fmt, v);
		wstring s(buf, buf + l);
		delete(buf);
		return s;
	}

	//////////////////////////////////////////////////////////////////////

	void Trace(wchar const *strMsg, ...)
	{
		va_list args;
		va_start(args, strMsg);
		OutputDebugStringW(Format_V(strMsg, args).c_str());
		va_end(args);
	}

	//////////////////////////////////////////////////////////////////////

	void Trace(char const *strMsg, ...)
	{
		va_list args;
		va_start(args, strMsg);
		OutputDebugStringA(Format_V(strMsg, args).c_str());
		va_end(args);
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
					delete[] buf;
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
		va_list v;
		va_start(v, fmt);
		return Format_V(fmt, v);
	}

	//////////////////////////////////////////////////////////////////////

	string Format(char const *fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		return Format_V(fmt, v);
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

	tstring TStringFromWideString(wstring const &str)
	{
#ifdef UNICODE
		return str;
#else
		return StringFromWideString(str);
#endif
	}

	//////////////////////////////////////////////////////////////////////

	string StringFromTString(tstring const &str)
	{
#ifdef UNICODE
		return StringFromWideString(str);
#else
		return str;
#endif
	}

	//////////////////////////////////////////////////////////////////////

	tstring TStringFromString(string const &str)
	{
#ifdef UNICODE
		return WideStringFromString(str);
#else
		return str;
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

	tstring ConcatenatePaths(tchar const *base, tchar const *relative)
	{
		// do all sorts of checks here...
		return tstring(base) + relative;
	}

	//////////////////////////////////////////////////////////////////////

	static struct PathComponents
	{
		tchar drive[_MAX_DRIVE];
		tchar dir[_MAX_DIR];
		tchar name[_MAX_FNAME];
		tchar ext[_MAX_EXT];
	} pc;

	//////////////////////////////////////////////////////////////////////

	PathComponents SplitPath(tchar const *path, PathComponents &pc)
	{
		_tsplitpath_s(path, pc.drive, pc.dir, pc.name, pc.ext);
		return pc;
	}

	//////////////////////////////////////////////////////////////////////

	tstring GetDrive(tchar const *path)
	{
		return SplitPath(path, pc).drive;
	}

	//////////////////////////////////////////////////////////////////////

	tstring GetPath(tchar const *path)
	{
		return SplitPath(path, pc).dir;
	}

	//////////////////////////////////////////////////////////////////////

	tstring GetFilename(tchar const *path)
	{
		return SplitPath(path, pc).name;
	}

	//////////////////////////////////////////////////////////////////////

	tstring GetExtension(tchar const *path)
	{
		return SplitPath(path, pc).ext;
	}

	//////////////////////////////////////////////////////////////////////

	tstring SetExtension(tchar const *path, tchar const *ext)
	{
		SplitPath(path, pc);
		return tstring(pc.drive) + pc.dir + pc.name + ext;
	}
	//////////////////////////////////////////////////////////////////////

	void ErrorMessageBox(tchar const *format, ...)
	{
		va_list v;
		va_start(v, format);
		tstring s = Format_V(format, v);
		MessageBox(NULL, s.c_str(), TEXT("Error"), MB_ICONEXCLAMATION);
		TRACE(Format(TEXT("%s\n"), s.c_str()).c_str());
		//assert(false);
	}

}