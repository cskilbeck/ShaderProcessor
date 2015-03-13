//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct File
	{
		~File();
		bool Open(tchar const *filename);
		bool Create(tchar const *filename);
		void Acquire(HANDLE handle);
		void Close();
		bool IsOpen() const;
		int64 Size();
		bool Read(uint32 bytes, void *buffer, uint32 &read);
		bool Write(uint32 bytes, void const *buffer, uint32 &wrote);

		operator HANDLE() const
		{
			return h.obj;
		}

		tstring name;
		Handle h;
		long error;
	};

	bool LoadFile(tchar const *filename, void **data, uint32 *size = null);
	bool SaveFile(tchar const *filename, void const *data, uint32 size);

	bool FileOrFolderExists(tchar const *filename);
	bool FolderExists(tchar const *foldername);
	bool FileExists(tchar const *filename);
}
