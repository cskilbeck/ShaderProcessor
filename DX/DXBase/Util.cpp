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
		Ptr<char> buf(new char[l + 1]);
		l = _vsnprintf_s(buf.get(), l + 1, _TRUNCATE, fmt, v);
		return string(buf.get(), buf.get() + l);
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
		Ptr<wchar> buf(new wchar[l + 1]);
		l = _vsnwprintf_s(buf.get(), l + 1, _TRUNCATE, fmt, v);
		return wstring(buf.get(), buf.get() + l);
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

	//HRESULT LoadResource(uint32 resourceid, void **data, size_t *size)
	//{
	//	HRSRC myResource = ::FindResource(NULL, MAKEINTRESOURCE(resourceid), RT_RCDATA);
	//	if(myResource == null)
	//	{
	//		return ERROR_RESOURCE_DATA_NOT_FOUND;
	//	}

	//	HGLOBAL myResourceData = ::LoadResource(NULL, myResource);
	//	if(myResourceData == null)
	//	{
	//		return ERROR_RESOURCE_FAILED;
	//	}

	//	void *pMyBinaryData = ::LockResource(myResourceData);
	//	if(pMyBinaryData == null)
	//	{
	//		return ERROR_RESOURCE_NOT_AVAILABLE;
	//	}

	//	size_t s = (size_t)SizeofResource(GetModuleHandle(null), myResource);
	//	if(s == 0)
	//	{
	//		return ERROR_RESOURCE_FAILED;
	//	}

	//	if(size != null)
	//	{
	//		*size = s;
	//	}
	//	if(data != null)
	//	{
	//		*data = pMyBinaryData;
	//	}
	//	return S_OK;
	//}

	//////////////////////////////////////////////////////////////////////
	// TODO (charlie): get rid of this crap

	//uint8 *LoadFile(tchar const *filename, uint64 *size = null)
	//{
	//	Ptr<uint8> buf;
	//	DiskFile f;
	//	if(!f.Open(filename, DiskFile::ForReading))
	//	{
	//		MessageBox(null, Format(TEXT("File not found: %s"), filename).c_str(), TEXT("LoadFile"), MB_ICONERROR);
	//		return null;
	//	}

	//	uint64 fileSize;
	//	if(f.GetSize(fileSize) != S_OK)
	//	{
	//		MessageBox(null, Format(TEXT("Couldn't get file size of: %s"), filename).c_str(), TEXT("LoadFile"), MB_ICONERROR);
	//		return null;
	//	}

	//	buf.reset(new byte[fileSize]);

	//	uint64 got;
	//	if(f.Read(buf.get(), fileSize, &got) != S_OK || got != fileSize)
	//	{
	//		MessageBox(null, Format(TEXT("Error reading from: %s"), filename).c_str(), TEXT("LoadFile"), MB_ICONERROR);
	//		return null;
	//	}
	//	if(size != null)
	//	{
	//		*size = fileSize;
	//	}
	//	return buf.release();
	//}

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
		GetCurrentDirectory(l, s.data());
		s[l] = 0;
		return string(s.data());
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

	//////////////////////////////////////////////////////////////////////
	// parallel lines never return true, even if horizontal or vertical...

	bool LineIntersect(Vec2f const &p0, Vec2f const &p1, Vec2f const &p2, Vec2f const &p3, Vec2f *intersectionPoint)
	{
		Vec2f s10 = p1 - p0;
		Vec2f s32 = p3 - p2;
		float d = s10.Cross(s32);
		if(d == 0)
		{
			return false;
		}
		bool dp = d > 0;
		Vec2f s02 = p0 - p2;
		float s = s10.Cross(s02);
		if((s < 0) == dp)
		{
			return false;
		}
		float t = s32.Cross(s02);
		if((t < 0) == dp || (s > d) == dp || (t > d) == dp)
		{
			return false;
		}
		if(intersectionPoint != null)
		{
			*intersectionPoint = p0 + ((t / d) * s10);
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////

	float UnitDistanceToLine(Vec2f const &a, Vec2f const &b, Vec2f const &p)
	{
		return Vec2f(b.y - a.y, a.x - b.x).Normalize().Dot(p - a);
	}

	//////////////////////////////////////////////////////////////////////

	float UnscaledDistanceToLine(Vec2f const &a, Vec2f const &b, Vec2f const &p)
	{
		return Vec2f(b.y - a.y, a.x - b.x).Dot(p - a);
	}

	//////////////////////////////////////////////////////////////////////

	bool PointInRectangle(Vec2f const &point, Vec2f const r[4], float const margins[4])
	{
		return
			UnitDistanceToLine(r[1], r[0], point) >= -margins[1] &&
			UnitDistanceToLine(r[2], r[1], point) > -margins[2] &&
			UnitDistanceToLine(r[3], r[2], point) > -margins[3] &&
			UnitDistanceToLine(r[0], r[3], point) >= -margins[0];
	}

	//////////////////////////////////////////////////////////////////////

	bool PointInRectangle(Vec2f const &point, Vec2f const r[4])
	{
		return
			UnscaledDistanceToLine(r[1], r[0], point) >= 0 &&
			UnscaledDistanceToLine(r[2], r[1], point) > 0 &&
			UnscaledDistanceToLine(r[3], r[2], point) > 0 &&
			UnscaledDistanceToLine(r[0], r[3], point) >= 0;
	}

	//////////////////////////////////////////////////////////////////////
	// Return minimum distance between line segment ab and point p

	float DistanceToLineSegmentSquared(Vec2f const &v, Vec2f const &w, Vec2f const &c)
	{
		Vec2f vw(w - v);
		float t = (c - v).Dot(vw) / vw.LengthSquared();
		return (((t <= 0) ? v : (t >= 1) ? w : (v + t * vw)) - c).LengthSquared();
	}

	//////////////////////////////////////////////////////////////////////
	// Return minimum distance between line segment ab and point p

	float DistanceToLineSegment(Vec2f const &a, Vec2f const &b, Vec2f const &p)
	{
		return sqrtf(DistanceToLineSegmentSquared(a, b, p));
	}

	//////////////////////////////////////////////////////////////////////

	bool RectanglesOverlap(Vec2f const a[4], Vec2f const b[4])
	{
		// any points inside the other?
		for(uint i = 0; i < 4; ++i)
		{
			if(PointInRectangle(a[i], b) || PointInRectangle(b[i], a))
			{
				return true;
			}
		}
		// so, do any of the lines of one intersect any of the lines of the other?
		for(uint i = 0; i < 4; ++i)
		{
			uint j = (i + 1) % 4;
			Vec2f sidea = a[j] - a[i];
			for(uint k = 0; k < 4; ++k)
			{
				uint l = (k + 1) % 4;
				Vec2f sideb = b[l] - b[k];
				if(LineIntersect(a[i], a[j], b[k], b[l], null))
				{
					return true;
				}
			}
		}
		return false;
	}
}