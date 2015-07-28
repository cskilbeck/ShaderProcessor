//////////////////////////////////////////////////////////////////////

#pragma  once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct LinearAllocator
	{
		//////////////////////////////////////////////////////////////////////

		LinearAllocator(size_t size)
		{
			Resize(size);
		}

		//////////////////////////////////////////////////////////////////////

		~LinearAllocator()
		{
		}

		//////////////////////////////////////////////////////////////////////

		void Resize(size_t size)
		{
			mSize = size;
			mBuffer.reset(new uint8[size]);
		}

		//////////////////////////////////////////////////////////////////////

		size_t BytesUsedSoFar() const
		{
			return mPointer - mBuffer.get();
		}

		//////////////////////////////////////////////////////////////////////

		size_t BytesRemaining() const
		{
			return mSize - BytesUsedSoFar();
		}

		//////////////////////////////////////////////////////////////////////

		void *Allocate(size_t size)
		{
			if(BytesRemaining() < size)
			{
				return null;
			}
			uint8 *p = mPointer;
			mPointer += size;
			return (void *)p;
		}

		//////////////////////////////////////////////////////////////////////

		size_t		mSize;
		Ptr<uint8>	mBuffer;
		uint8 *		mPointer;
	};
}