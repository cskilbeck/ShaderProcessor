//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace
{
	int GetFileNameFromHandle(HANDLE hFile, DX::tstring &name)
	{
		using namespace DX;
		vector<tchar> buffer;
		DWORD len = GetFinalPathNameByHandle(hFile, null, 0, VOLUME_NAME_DOS);
		if(len == 0)
		{
			return GetLastError();
		}
		buffer.resize(len + 1);
		len =  GetFinalPathNameByHandle(hFile, buffer.data(), (DWORD)buffer.size(), VOLUME_NAME_DOS);
		if(len == 0)
		{
			return GetLastError();
		}
		tchar *p = buffer.data();
		if(_tcsncmp(p, TEXT("\\\\?\\"), 4) == 0)
		{
			p += 4;
		}
		name = p;
		return S_OK;
	}
}

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct FileBase
	{
		FileBase()
		{
		}

		virtual ~FileBase()
		{
		}

		virtual int Read(void *buffer, uint64 size, uint64 *got = null) = 0;
		virtual int Write(void const *buffer, uint64 size, uint64 *wrote = null) = 0;
		virtual int Seek(size_t offset, int seekType, intptr *newPosition = null) = 0;
		virtual int Reopen(FileBase **other) = 0;	// make a new one of the same type on the same file
		virtual int GetPosition(uint64 &position) = 0;
		virtual int GetSize(uint64 &size) = 0;
		virtual void Close() = 0;
		virtual tstring Name() = 0;

		int Load(Blob &result)
		{
			uint64 size;
			DXR(GetSize(size));
			Ptr<byte> buffer(new byte[size]);
			uint64 got;
			DXR(Read(buffer.get(), size, &got));
			result.Reset(buffer.release(), size);
			return true;
		}

		template<typename T> int Get(T &b)
		{
			uint64 got;
			DXR(Read(&b, (uint32)sizeof(b), &got));
			return (got == sizeof(b)) ? S_OK : E_FAIL;
		}

		template <typename T, typename U> int GetAs(U &b)
		{
			T temp;
			DXR(Get(temp));
			b = (U)temp;
			return S_OK;
		}

		template<typename T> int GetUInt16(T &b)
		{
			DXR(GetAs<uint16>(b));
			return S_OK;
		}

		template<typename T> int GetUInt32(T &b)
		{
			DXR(GetAs<uint32>(b));
			return S_OK;
		}

		template<typename T> int GetUInt64(T &b)
		{
			DXR(GetAs<uint64>(b));
			return S_OK;
		}
	};

	//////////////////////////////////////////////////////////////////////
	// NemoryFile doesn't take ownership of the data - it's up the client
	// to free it

	struct MemoryFile: FileBase
	{
		MemoryFile(size_t s)
			: ptr(new byte[s])
			, size(s)
			, pos(ptr)
			, own(true)
		{
		}

		MemoryFile(void *p = null, size_t s = 0)
			: ptr((byte *)p)
			, size(s)
			, pos(ptr)
			, own(false)
		{
		}

		~MemoryFile()
		{
			Close();
		}

		void Reset(size_t s)
		{
			Delete(ptr);
			ptr = new byte[s];
			size = s;
			pos = ptr;
			own = true;
		}

		void Reset(void *p, size_t s)
		{
			Delete(ptr);
			ptr = (byte *)p;
			size = s;
			pos = ptr;
			own = true;
		}

		tstring Name() override
		{
			return Format(TEXT("MemoryFile:%p\\%s"), ptr, size);
		}

		Byte *End() const
		{
			return ptr + size;
		}

		int GetPosition(uint64 &position) override
		{
			position = (uint64)(pos - ptr);
			return S_OK;
		}

		int Reopen(FileBase **other)
		{
			*other = null;
			return ERROR_NOT_SUPPORTED;
			//MemoryFile *n = new MemoryFile();
			//n->ptr = ptr;
			//n->size = size;
			//n->pos = n->ptr;
			//*other = n;
			//return true;
		}

		int Seek(size_t offset, int seekType, intptr *newPosition = null) override
		{
			switch(seekType)
			{
				case SEEK_SET:
					pos = ptr + offset;
					break;

				case SEEK_CUR:
					pos += offset;
					break;

				case SEEK_END:
					pos = ptr + size - offset;
					break;
			}
			pos = max(0, min(End(), pos));
			if(newPosition != null)
			{
				*newPosition = pos - ptr;
			}
			return S_OK;
		}

		int GetSize(uint64 &fileSize) override
		{
			fileSize = size;
			return S_OK;
		}

		void Close() override
		{
			if(own)
			{
				Delete(ptr);
				pos = null;
				size = 0;
			}
		}

		int Read(void *buffer, uint64 size, uint64 *got = null) override
		{
			uint64 position;
			DXR(GetPosition(position));
			uint64 remain = size - position;
			uint64 get = min(remain, size);
			memcpy(buffer, pos, get);
			pos += get;
			if(got != null)
			{
				*got = get;
			}
			return (get == size) ? S_OK : E_BOUNDS;
		}

		int Write(void const *buffer, uint64 amount, uint64 *wrote = null) override
		{
			uint64 position;
			DXR(GetPosition(position));
			size_t remain = min(amount, size - position);
			memcpy(pos, buffer, remain);
			pos += remain;
			if(wrote != null)
			{
				*wrote = remain;
			}
			return S_OK;
		}

		Byte *ptr;
		uint64 size;
		Byte *pos;
		bool own;
	};

	//////////////////////////////////////////////////////////////////////

	struct DiskFile: FileBase
	{
		// Open existing file for reading
		// Open existing file for writing
		// Create new file for writing (overwrite)
		// Create new file for writing (don't overwrite)
		
		enum Mode
		{
			ForReading = 1,
			ForWriting = 2
		};

		enum OverwriteOption
		{
			Overwrite,
			DontOverwrite
		};

		DiskFile()
		{
		}

		~DiskFile() override
		{
			Release();
		}

		void Borrow(Handle handle)
		{
			owned = false;
			h = handle;
		}

		void Acquire(Handle handle)
		{
			owned = true;
			h = handle;
		}

		void Relinquish()
		{
			owned = false;
		}

		void Release()
		{
			if(owned)
			{
				h.Release();
			}
			h = Handle();
		}

		operator Handle()
		{
			return h;
		}

		int Open(tchar const *name, Mode mode)
		{
			DWORD access = 0;
			DWORD share = 0;

			switch(mode)
			{
				case ForReading:
					access = GENERIC_READ;
					share = FILE_SHARE_READ;
					break;
				case ForWriting:
					access = GENERIC_WRITE;
					share = FILE_SHARE_WRITE;
					break;
			}
			Trace(TEXT("Open(%s)\n"), name);
			h = CreateFile(name, access, share, null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null);
			if(!h.IsValid())
			{
				error = GetLastError();
				Trace(TEXT("Failed to open %s (%08x)\n"), name, error);
			}
			owned = true;
			return h.IsValid() ? S_OK : HRESULT_FROM_WIN32(error);
		}

		int Create(char const *name, OverwriteOption overwrite)
		{
			DWORD creation;
			switch(overwrite)
			{
				case Overwrite:
					creation = CREATE_ALWAYS;
					break;
				case DontOverwrite:
					creation = CREATE_NEW;
					break;

			}
			h = CreateFile(name, GENERIC_WRITE, FILE_SHARE_WRITE, null, creation, FILE_ATTRIBUTE_NORMAL, null);
			if(!h.IsValid())
			{
				error = GetLastError();
			}
			owned = true;
			return h.IsValid() ? S_OK : HRESULT_FROM_WIN32(error);
		}

		tstring Name() override
		{
			tstring filename;
			return GetFileNameFromHandle(h, filename) == S_OK ? filename : TEXT("??");
		}

		// Reopen always returns a read only handle for now...
		int Reopen(FileBase **other)
		{
			DiskFile *f = new DiskFile();
			*other = f;
			return f->Open(Name().c_str(), ForReading);
		}

		int Read(void *buffer, uint64 size, uint64 *got = null) override
		{
			uint64 total = 0;
			while(total < size)
			{
				DWORD localGot;
				uint32 get = (uint32)min(size, MAXUINT32);
				if(ReadFile(h, buffer, get, &localGot, null) == 0)
				{
					error = GetLastError();
					return HRESULT_FROM_WIN32(error);
				}
				total += localGot;
			}
			if(got != null)
			{
				*got = total;
			}
			return S_OK;
		}

		int Write(void const *buffer, uint64 size, uint64 *wrote = null) override
		{
			uint64 total = 0;
			uint64 remain = size;
			while(remain > 0)
			{
				DWORD localWrote;
				DWORD w = (DWORD)min(MAXUINT32, remain);
				if(WriteFile(h, buffer, w, &localWrote, null) == 0)
				{
					error = GetLastError();
					return HRESULT_FROM_WIN32(error);
				}
				total += localWrote;
				remain -= localWrote;
			}
			if(wrote != null)
			{
				*wrote = total;
			}
			return S_OK;
		}

		int Seek(size_t offset, int seekType, intptr *newPosition) override
		{
			LARGE_INTEGER l = { offset & MAXUINT32, offset >> 32 };
			if(SetFilePointerEx(h, l, (PLARGE_INTEGER)newPosition, (DWORD)seekType) == 0)
			{
				error = GetLastError();
				return HRESULT_FROM_WIN32(error);
			}
			return S_OK;
		}

		int GetSize(uint64 &size) override
		{
			LARGE_INTEGER s;
			if(GetFileSizeEx(h, &s) == 0)
			{
				error = GetLastError();
				return HRESULT_FROM_WIN32(error);
			}
			size = (uint64)s.QuadPart;
			return S_OK;
		}

		int GetPosition(uint64 &position) override
		{
			intptr newPos;
			if(Seek(0, SEEK_CUR, &newPos) == 0)
			{
				error = GetLastError();
				return HRESULT_FROM_WIN32(error);
			}
			position = (uint64)newPos;
			return S_OK;
		}

		void Close() override
		{
			h.Close();
		}

		Handle h;
		bool owned;
		DWORD error;
	};

	HRESULT LoadResource(uint32 resourceid, void **data, size_t *size);

	int LoadFile(tchar const *filename, MemoryFile &data);
	int SaveFile(tchar const *filename, void const *data, uint32 size);

	bool FileOrFolderExists(tchar const *filename);
	bool FolderExists(tchar const *foldername);
	bool FileExists(tchar const *filename);
}
