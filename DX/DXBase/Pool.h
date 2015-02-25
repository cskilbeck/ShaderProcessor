//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	template <typename T> struct Pool
	{
		struct Dummy: list_node < Dummy >
		{
		};

		static_assert(sizeof(T) >= sizeof(list_node<Dummy>), "Pooled objects must be at least as big as list_node");
		static_assert(sizeof(Dummy) == sizeof(list_node<Dummy>), "Compiler has inserted some data which isn't helping...");

		linked_list<Dummy>	mFreeList;
		T *						mPool;

		Pool(uint n)
		{
			mPool = new T[n];
			for(uint i = 0; i < n; ++i)
			{
				mFreeList.push_back((Dummy &)mPool[i]);
			}
		}

		~Pool()
		{
			delete[] mPool;
		}

		T &Alloc()
		{
			assert(!mFreeList.empty());
			Dummy *d = mFreeList.pop_back();
			return (T &)*d;
		}

		void Free(T &p)
		{
			mFreeList.push_back((Dummy &)p);
		}
	};
}
