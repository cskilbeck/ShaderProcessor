//////////////////////////////////////////////////////////////////////

#include "DXBase.h"

//////////////////////////////////////////////////////////////////////

namespace DX
{
	bool File::Open(tchar const *filename)
	{
		name = filename;
		h = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if(h == INVALID_HANDLE_VALUE)
		{
			TRACE(TEXT("Error opening %s\n"), filename);
			error = ErrorMsgBox(Format(TEXT("Error opening %s"), filename).c_str());
		}
		TRACE(TEXT("Opened %s\n"), filename);
		return h.IsValid();
	}

	//////////////////////////////////////////////////////////////////////

	void File::Acquire(HANDLE handle)
	{
		name = TEXT("Unknown");
		h = handle;
	}

	//////////////////////////////////////////////////////////////////////

	bool File::Create(tchar const *filename)
	{
		name = filename;
		h = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, null, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if(h == INVALID_HANDLE_VALUE)
		{
			TRACE(TEXT("Error creating %s\n"), filename);
			error = ErrorMsgBox(Format(TEXT("Error opening %s"), filename).c_str());
		}
		TRACE(TEXT("Created %s\n"), filename);
		return h.IsValid();
	}

	//////////////////////////////////////////////////////////////////////

	File::~File()
	{
		Close();
	}

	//////////////////////////////////////////////////////////////////////

	void File::Close()
	{
		TRACE(TEXT("Closing %s\n"), name.c_str());
		h.Close();
	}

	//////////////////////////////////////////////////////////////////////

	bool File::IsOpen() const
	{
		return h.IsValid();
	}

	//////////////////////////////////////////////////////////////////////

	int64 File::Size()
	{
		if(!IsOpen())
		{
			error = ERROR_FILE_OFFLINE;
			return -1;
		}
		int64 fileSize = 0;
		if(GetFileSizeEx(h, (LARGE_INTEGER *)&fileSize))
		{
			error = ERROR_SUCCESS;
			TRACE(TEXT("Size of %s is %ld bytes\n"), name.c_str(), fileSize);
			return fileSize;
		}
		error = ErrorMsgBox(Format(TEXT("Error getting file size of %s"), name.c_str()).c_str());
		return -1;
	}

	//////////////////////////////////////////////////////////////////////

	bool File::Read(uint32 bytes, void *buffer, uint32 &read)
	{
		error = ERROR_SUCCESS;
		if(ReadFile(h, buffer, bytes, (DWORD *)&read, null))
		{
			TRACE(TEXT("Read %d bytes from %s\n"), read, name.c_str());
			return true;
		}
		error = GetLastError();
		TRACE(TEXT("Error %08x reading %d bytes from %s\n"), error, bytes, name.c_str());
		return false;
	}

	//////////////////////////////////////////////////////////////////////

	bool File::Write(uint32 bytes, void const *buffer, uint32 &wrote)
	{
		error = ERROR_SUCCESS;
		if(WriteFile(h, buffer, bytes, (DWORD *)&wrote, null))
		{
			TRACE(TEXT("Wrote %d bytes to %s\n"), wrote, name.c_str());
			return true;
		}
		error = GetLastError();
		TRACE(TEXT("Error %08x writing %d bytes to %s\n"), error, bytes, name.c_str());
		return false;
	}

	//////////////////////////////////////////////////////////////////////
	// Simple file loader

	bool LoadFile(tchar const *filename, void **data, uint32 *size)
	{
		assert(filename != null && data != null && size != null);

		File f;
		if(!f.Open(filename))
		{
			return false;
		}

		uint64 fileSize = f.Size();
		if(fileSize > 0x7fffffff - sizeof(tchar))
		{
			ErrorMsgBox(Format(TEXT("Can't load %s (~2GB limit, sorry)"), filename).c_str());
			return false;
		}

		Ptr<byte> buf(new byte[(size_t)(fileSize + sizeof(tchar))]);
		if(buf == null)
		{
			ErrorMsgBox(Format(TEXT("Can't load %s - allocation failed"), filename).c_str());
			return false;
		}

		uint32 got;
		if(!f.Read((uint32)fileSize, buf.get(), got))
		{
			return false;
		}

		*(tchar *)(buf.get() + fileSize) = tchar(0);

		*data = buf.release();
		*size = (uint32)fileSize;
		return true;
	}

	//////////////////////////////////////////////////////////////////////
	// Simple file saver

	bool SaveFile(tchar const *filename, void const *data, uint32 size)
	{
		assert(filename != null && data != null && size != 0);
		File f;
		if(!f.Create(filename))
		{
			return false;
		}
		return f.Write((uint32)size, data, size);
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