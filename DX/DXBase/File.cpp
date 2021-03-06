//////////////////////////////////////////////////////////////////////

#include "DXBase.h"
#include <Shlobj.h>
#pragma comment(lib, "shell32.lib")

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////
	// Load a binary resource from the exe

	HRESULT LoadResource(uint32 resourceid, void **data, uint64 *size)
	{
	    HRSRC myResource = ::FindResource(NULL, MAKEINTRESOURCE(resourceid), RT_RCDATA);
	    if(myResource == null)
	    {
			return HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND);
	    }

	    HGLOBAL myResourceData = ::LoadResource(NULL, myResource);
	    if(myResourceData == null)
	    {
			return HRESULT_FROM_WIN32(ERROR_RESOURCE_FAILED);
	    }

	    void *pMyBinaryData = ::LockResource(myResourceData);
	    if(pMyBinaryData == null)
	    {
			return HRESULT_FROM_WIN32(ERROR_RESOURCE_NOT_AVAILABLE);
	    }

		uint64 s = (uint64)SizeofResource(GetModuleHandle(null), myResource);
	    if(s == 0)
	    {
			return HRESULT_FROM_WIN32(ERROR_RESOURCE_FAILED);
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

	bool FileOrFolderExists(tchar const *filename)
	{
		return GetFileAttributes(filename) != INVALID_FILE_ATTRIBUTES;
	}

	//////////////////////////////////////////////////////////////////////

	bool FileExists(tchar const *filename)
	{
		DWORD dwAttrib = GetFileAttributes(filename);
		return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0;
	}

	//////////////////////////////////////////////////////////////////////

	bool FolderExists(tchar const *foldername)
	{
		DWORD dwAttrib = GetFileAttributes(foldername);
		return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	//////////////////////////////////////////////////////////////////////

	int CreateFolder(tchar const *name)
	{
		int r = SHCreateDirectory(null, WString(name).c_str());
		return (r == ERROR_SUCCESS ||
				r == ERROR_ALREADY_EXISTS ||
				r == ERROR_FILE_EXISTS)
			? S_OK : HRESULT_FROM_WIN32(r);
	}

	//////////////////////////////////////////////////////////////////////
	// Simple file loader
	// TODO (charlie): get rid of this, use AssetManager::LoadFile

	int LoadFile(tchar const *filename, MemoryFile &data)
	{
		DiskFile f;
		uint64 fileSize;
		DXR(f.Open(filename, DiskFile::ForReading));
		DXR(f.GetSize(fileSize));

		MemoryFile mf(fileSize + sizeof(tchar));
		if(mf.ptr == null)
		{
			ErrorMsgBox(Format(TEXT("Can't load %s - allocation failed"), filename).c_str());
			return false;
		}

		uint64 got;
		DXR(f.Read(mf.ptr, fileSize, &got));
		*(tchar *)(mf.ptr + fileSize) = tchar(0);
		data = mf;	// transfer ownership to the client MemoryFile
		mf.own = false;
		return true;
	}

	//////////////////////////////////////////////////////////////////////
	// Simple file saver

	int SaveFile(tchar const *filename, void const *data, uint32 size)
	{
		// Check the folder exists
		tstring folderName = GetPath(filename);
		if(!FolderExists(folderName.c_str()))
		{
			// If it doesn't, create it
			DXR(CreateFolder(folderName.c_str()));
		}

		DiskFile f;
		DXR(f.Create(filename, DiskFile::Overwrite));
		DXR(f.Write(data, size));
		return S_OK;
	}
}