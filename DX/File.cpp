//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

namespace DX
{

	bool File::Open(tchar const *filename)
	{
		name = filename;
		h = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if(h == INVALID_HANDLE_VALUE)
		{
			error = ErrorMsgBox(Format(TEXT("Error opening %s"), filename).c_str());
		}
		return h.IsValid();
	}

	//////////////////////////////////////////////////////////////////////

	void File::Close()
	{
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
			return fileSize;
		}
		error = ErrorMsgBox(Format(TEXT("Error getting file size of %s"), name.c_str()).c_str());
		return -1;
	}

	//////////////////////////////////////////////////////////////////////

	uint32 File::Read(uint32 bytes, void *buffer)
	{
		DWORD got;
		error = ERROR_SUCCESS;
		if(ReadFile(h, buffer, bytes, &got, null))
		{
			return got;
		}
		error = GetLastError();
		return 0;
	}

	//////////////////////////////////////////////////////////////////////

	uint32 File::Write(uint32 bytes, void *buffer)
	{
		DWORD wrote;
		error = ERROR_SUCCESS;
		if(WriteFile(h, buffer, bytes, &wrote, null))
		{
			return wrote;
		}
		error = GetLastError();
		return 0;
	}

	//////////////////////////////////////////////////////////////////////
	// Simple file loader

	bool LoadFile(tchar const *filename, void **data, uint32 *size)
	{
		assert(filename != null && data != null && size != null);

		File f;
		if(!f.Open(filename))
		{
			ErrorMsgBox(Format(TEXT("Error opening %s"), filename).c_str());
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

		if(!f.Read((uint32)fileSize, buf.get()))
		{
			return false;
		}

		*(tchar *)(buf.get() + fileSize) = tchar(0);

		*data = buf.release();
		*size = (uint32)fileSize;
		return true;
	}

}