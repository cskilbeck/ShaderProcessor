//////////////////////////////////////////////////////////////////////

#include "DXBase.h"

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////
	// Load a binary resource from the exe

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
	// Simple file loader

	bool LoadFile(tchar const *filename, MemoryFile &data)
	{
		DiskFile f;
		if(!f.Open(filename, DiskFile::ForReading))
		{
			return false;
		}

		intptr fileSize = f.Size();
		if(fileSize == -1)
		{
			ErrorMsgBox(Format(TEXT("Can't load %s (~2GB limit, sorry)"), filename).c_str());
			return false;
		}

		MemoryFile mf(fileSize + sizeof(tchar));
		if(mf.ptr == null)
		{
			ErrorMsgBox(Format(TEXT("Can't load %s - allocation failed"), filename).c_str());
			return false;
		}

		uint64 got;
		if(!f.Read(mf.ptr, fileSize, &got))
		{
			return false;
		}

		*(tchar *)(mf.ptr + fileSize) = tchar(0);

		data = mf;	// transfer ownership to the client MemoryFile
		mf.own = false;
		return true;
	}

	//////////////////////////////////////////////////////////////////////
	// Simple file saver

	bool SaveFile(tchar const *filename, void const *data, uint32 size)
	{
		assert(filename != null && data != null && size != 0);
		DiskFile f;
		if(!f.Create(filename, DiskFile::Overwrite))
		{
			return false;
		}
		return f.Write(data, size);
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
}