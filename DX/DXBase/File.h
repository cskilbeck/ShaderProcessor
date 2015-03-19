//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	template<typename T> struct Flags
	{
		Flags(T b = 0)
			: bits(b)
		{
		}

		operator T() const
		{
			return bits;
		}

		bool AreAllSet(T mask) const
		{
			return (bits & mask) == mask;
		}

		bool IsAnySet(T mask) const
		{
			return (bits & mask) != 0;
		}

		T Set(T mask)
		{
			bits |= mask;
			return bits;
		}

		T Clear(T mask)
		{
			bits &= ~mask;
			return bits;
		}

		bool operator() (T mask) const
		{
			return IsAnySet(mask);
		}

		T operator += (T mask)
		{
			return Set(mask);
		}

		T operator -= (T mask)
		{
			return Clear(mask);
		}

		T bits;
	};

	//////////////////////////////////////////////////////////////////////

	using Flags8 = Flags < uint8 >;
	using Flags16 = Flags < uint16 > ;
	using Flags32 = Flags < uint32 > ;
	using Flags64 = Flags < uint64 > ;

	//////////////////////////////////////////////////////////////////////

	struct FileBase
	{
		FileBase();
		virtual ~FileBase();
		virtual bool Read(void *buffer, uint32 size, uint32 *got = null) = 0;
		virtual bool Write(void const *buffer, uint32 size, uint32 *wrote = null) = 0;
		virtual bool Seek(size_t offset, int seekType) = 0;
		virtual intptr Position() const = 0;
		virtual intptr Size() const = 0;
		virtual void Close() = 0;

		template<typename T> bool Get(T &b)
		{
			size_t got;
			return Read(&b, sizeof(b), &got) && got == sizeof(b);
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct MemoryFile: FileBase
	{
		MemoryFile(void *p = null, size_t s = 0)
			: ptr(reinterpret_cast<Byte *>(p))
			, size(s)
		{
		}

		~MemoryFile()
		{
		}

		Byte *End() const
		{
			return ptr + size;
		}

		intptr Position() const override
		{
			return pos - ptr;
		}

		bool Seek(size_t offset, int seekType) override
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
			return true;
		}

		intptr Size() const override
		{
			return size;
		}

		void Close() override
		{
		}

		bool Read(void *buffer, uint32 size, uint32 *got = null) override
		{
			intptr remain = size - Position();
			intptr get = min(remain, size);
			memcpy(buffer, pos, (uint32)get);
			pos += get;
			if(got != null)
			{
				*got = (uint32)get;
			}
			return get == size;
		}

		bool Write(void const *buffer, uint32 size, uint32 *wrote = null) override
		{
			return false;
		}

		Byte *ptr;
		Byte *pos;
		size_t size;
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

		void Acquire(Handle handle)
		{
			owned = false;
			h = handle;
		}

		void Release()
		{
			if(owned)
			{
				h.Release();
			}
			h = Handle();
		}

		bool Open(tchar const *name, Mode mode)
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
			h = CreateFile(name, access, share, null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null);
			owned = true;
			return h.IsValid();
		}

		bool Create(char const *name, OverwriteOption overwrite)
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
			owned = true;
			return h.IsValid();
		}

		bool Read(void *buffer, uint32 size, uint32 *got = null) override
		{
			return ReadFile(h, buffer, size, (LPDWORD)got, null) != 0;
		}

		bool Write(void const *buffer, uint32 size, uint32 *wrote = null) override
		{
			return WriteFile(h, buffer, size, (LPDWORD)wrote, null) != 0;
		}

		bool Seek(size_t offset, int seekType) override
		{
			LONG lo = (LONG)(offset >> 32);
			return SetFilePointer(h, (uint32)offset, &lo, seekType) != INVALID_SET_FILE_POINTER;
		}

		intptr Size() const override
		{
			HANDLE handle = const_cast<HANDLE>(h.obj);	// yuck
			DWORD hi;
			DWORD lo = GetFileSize(handle, &hi);
			return (lo != INVALID_FILE_SIZE) ? ((intptr)hi << 32) | lo : -1;
		}

		intptr Position() const override
		{
			HANDLE handle = const_cast<HANDLE>(h.obj);	// yuck
			LONG hi = 0;
			LONG lo = SetFilePointer(handle, 0, &lo, SEEK_SET);
			return (lo != INVALID_SET_FILE_POINTER) ? ((intptr)hi << 32) | lo : -1;
		}

		void Close() override
		{
			h.Close();
		}

		Handle h;
		bool owned;
	};

	//////////////////////////////////////////////////////////////////////

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
