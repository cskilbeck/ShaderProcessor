//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace
{
	bool GetFileNameFromHandle(HANDLE hFile, DX::tstring &name)
	{
		using namespace DX;
		vector<tchar> buffer;
		DWORD len = GetFinalPathNameByHandle(hFile, null, 0, VOLUME_NAME_DOS);
		if(len == 0)
		{
			return false;
		}
		buffer.resize(len + 1);
		len =  GetFinalPathNameByHandle(hFile, buffer.data(), (DWORD)buffer.size(), VOLUME_NAME_DOS);
		if(len == 0)
		{
			return false;
		}
		name = (_tcsncmp(buffer.data(), TEXT("\\\\?\\"), 4) == 0) ? buffer.data() + 4 : buffer.data();
		return true;
	}
}

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
		FileBase()
		{
		}

		virtual ~FileBase()
		{
		}

		virtual bool Read(void *buffer, uint64 size, uint64 *got = null) = 0;
		virtual bool Write(void const *buffer, uint64 size, uint64 *wrote = null) = 0;
		virtual bool Seek(size_t offset, int seekType, intptr *newPosition = null) = 0;
		virtual bool Reopen(FileBase **other) = 0;	// make a new one of the same type on the same file
		virtual intptr Position() = 0;
		virtual intptr Size() = 0;
		virtual void Close() = 0;
		virtual tstring Name() = 0;

		bool Load(Blob &result)
		{
			Ptr<byte> buffer(new byte[Size()]);
			uint64 got;
			if(!Read(buffer.get(), Size(), &got))
			{
				return false;
			}
			result.Reset(buffer.release(), Size());
			return true;
		}

		template<typename T> bool Get(T &b)
		{
			uint64 got;
			return Read(&b, (uint32)sizeof(b), &got) && got == sizeof(b);
		}

		template <typename T, typename U> bool GetAs(U &b)
		{
			T temp;
			if(!Get(temp))
			{
				return false;
			}
			b = (U)temp;
			return true;
		}

		template<typename T> bool GetUInt16(T &b)
		{
			return GetAs<uint16>(b);
		}

		template<typename T> bool GetUInt32(T &b)
		{
			return GetAs<uint32>(b);
		}

		template<typename T> bool GetUInt64(T &b)
		{
			return GetAs<uint64>(b);
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
			: ptr(reinterpret_cast<Byte *>(p))
			, size(s)
			, pos(ptr)
			, own(false)
		{
		}

		~MemoryFile()
		{
			Close();
		}

		tstring Name() override
		{
			return Format(TEXT("MemoryFile:%p\\%s"), ptr, size);
		}

		Byte *End() const
		{
			return ptr + size;
		}

		intptr Position() override
		{
			return pos - ptr;
		}

		bool Reopen(FileBase **other)
		{
			*other = null;
			return false;
			//MemoryFile *n = new MemoryFile();
			//n->ptr = ptr;
			//n->size = size;
			//n->pos = n->ptr;
			//*other = n;
			//return true;
		}

		bool Seek(size_t offset, int seekType, intptr *newPosition = null) override
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
			return true;
		}

		intptr Size() override
		{
			return size;
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

		bool Read(void *buffer, uint64 size, uint64 *got = null) override
		{
			uint64 remain = size - Position();
			uint64 get = min(remain, size);
			memcpy(buffer, pos, get);
			pos += get;
			if(got != null)
			{
				*got = get;
			}
			return get == size;
		}

		bool Write(void const *buffer, uint64 amount, uint64 *wrote = null) override
		{
			size_t remain = min(amount, size - Position());
			memcpy(pos, buffer, remain);
			if(wrote != null)
			{
				*wrote = remain;
			}
			return true;
		}

		Byte *ptr;
		size_t size;
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
			if(!h.IsValid())
			{
				error = GetLastError();
			}
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
			if(!h.IsValid())
			{
				error = GetLastError();
			}
			owned = true;
			return h.IsValid();
		}

		tstring Name() override
		{
			tstring filename;
			return GetFileNameFromHandle(h, filename) ? filename : TEXT("??");
		}

		// Reopen always returns a read only handle for now...
		bool Reopen(FileBase **other)
		{
			DiskFile *f = new DiskFile();
			*other = f;
			return f->Open(Name().c_str(), ForReading);
		}

		bool Read(void *buffer, uint64 size, uint64 *got = null) override
		{
			uint64 total = 0;
			while(total < size)
			{
				DWORD localGot;
				uint32 get = (uint32)min(size, MAXUINT32);
				if(ReadFile(h, buffer, get, &localGot, null) == 0)
				{
					error = GetLastError();
					return false;
				}
				total += localGot;
			}
			if(got != null)
			{
				*got = total;
			}
			return true;
		}

		bool Write(void const *buffer, uint64 size, uint64 *wrote = null) override
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
					return false;
				}
				total += localWrote;
				remain -= localWrote;
			}
			if(wrote != null)
			{
				*wrote = total;
			}
			return true;
		}

		bool Seek(size_t offset, int seekType, intptr *newPosition) override
		{
			LARGE_INTEGER l = { offset & MAXUINT32, offset >> 32 };
			if(SetFilePointerEx(h, l, (PLARGE_INTEGER)newPosition, (DWORD)seekType) == 0)
			{
				error = GetLastError();
				return false;
			}
			return true;
		}

		intptr Size() override
		{
			intptr size;
			if(GetFileSizeEx(h, (PLARGE_INTEGER)&size) == 0)
			{
				error = GetLastError();
				return -1;
			}
			return size;
		}

		intptr Position() override
		{
			intptr newPos;
			if(Seek(0, SEEK_CUR, &newPos) == 0)
			{
				error = GetLastError();
				return -1;
			}
			return newPos;
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

	bool LoadFile(tchar const *filename, MemoryFile &data);
	bool SaveFile(tchar const *filename, void const *data, uint32 size);

	bool FileOrFolderExists(tchar const *filename);
	bool FolderExists(tchar const *foldername);
	bool FileExists(tchar const *filename);
}
