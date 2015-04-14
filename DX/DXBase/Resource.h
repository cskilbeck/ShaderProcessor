//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Resource
	{
		Resource();

		Resource(void *data, uint64 size)
		{
			Data() = data;
			Size() = size;
		}

		virtual ~Resource();

		//////////////////////////////////////////////////////////////////////

		bool IsValid() const
		{
			return data != null;
		}

		//////////////////////////////////////////////////////////////////////

		operator void const *() const
		{
			return data;
		}

		//////////////////////////////////////////////////////////////////////

		void * &Data()
		{
			return data;
		}

		//////////////////////////////////////////////////////////////////////

		uint64 &Size()
		{
			return size;
		}

		//////////////////////////////////////////////////////////////////////

	protected:

		void *		data;
		uint64		size;
	};

	//////////////////////////////////////////////////////////////////////

	struct Blob
	{
		Blob()
			: mSize(0)
		{
		}

		Blob(uint64 size)
			: mSize(size)
			, mData(new Aligned_byte[size])
		{
		}

		void Fill(void *data, uint64 size)
		{
			memcpy(mData.get(), data, min(size, mSize));
		}

		void Reset(uint64 size)
		{
			mData.reset(new Aligned_byte[size]);
			mSize = size;
		}

		void Reset(byte *data, uint64 size)
		{
			assert(((intptr)data & 0xf) == 0);
			mData.reset((Aligned_byte *)data);
			mSize = size;
		}

		operator byte *()
		{
			return (byte *)mData.get();
		}

		uint64 Size() const
		{
			return mSize;
		}

		uint64 mSize;
		Ptr<Aligned_byte> mData;
	};

	//////////////////////////////////////////////////////////////////////

	struct MemoryResource: Resource
	{
		MemoryResource(void const *data, uint64 size)
			: Resource(const_cast<void *>(data), size)	// yuck
		{
		}

		~MemoryResource()
		{
			// We don't own the data in this case
			data = null;
			size = 0;
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct FileResource: Resource
	{
		FileResource();
		FileResource(void *data, uint64 size);
		FileResource(tchar const *name);
		void Set(void *data, uint64 size);	// takes ownership of the pointer
		~FileResource();
	};

	//////////////////////////////////////////////////////////////////////

	struct WinResource: Resource
	{
		WinResource(DWORD resourceID);
		~WinResource();
	};

}