//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Resource
	{
		Resource();
		Resource(void *data, size_t size)
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

		size_t &Size()
		{
			return size;
		}

		//////////////////////////////////////////////////////////////////////

	protected:

		void *		data;
		size_t		size;
	};

	//////////////////////////////////////////////////////////////////////

	struct Blob
	{
		Blob()
			: mSize(0)
		{
		}

		Blob(size_t size)
			: mSize(size)
			, mData(new byte[size])
		{
		}

		void Reset(byte *data, size_t size)
		{
			mData.reset(data);
			mSize = size;
		}

		operator byte *()
		{
			return mData.get();
		}

		size_t mSize;
		Ptr<byte> mData;
	};

	//////////////////////////////////////////////////////////////////////

	struct MemoryResource: Resource
	{
		MemoryResource(void const *data, size_t size)
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
		FileResource(tchar const *name);
		~FileResource();
	};

	//////////////////////////////////////////////////////////////////////

	struct WinResource: Resource
	{
		WinResource(DWORD resourceID);
		~WinResource();
	};

}