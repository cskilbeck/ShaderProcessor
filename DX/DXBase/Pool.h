//////////////////////////////////////////////////////////////////////
// TODO (charlie): make a nice way to override new() for pooled objects so you can just new() them without worrying about alloc/free

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	template <typename T, uint alignment = 16> struct Pool
	{
		using list_node = chs::list_node;
		template <typename T> using linked_list = chs::linked_list < T > ;

		struct Dummy: list_node
		{
		};

		static_assert(sizeof(T) >= sizeof(list_node), "Pooled objects must be at least as big as list_node");
		static_assert(sizeof(Dummy) == sizeof(list_node), "Compiler has inserted some data which isn't helping...");

		linked_list<Dummy>		mFreeList;
		T *						mPool;

		Pool(uint n)
		{
			mPool = (T *)_aligned_malloc(sizeof(T) * n, alignment);
			for(uint i = 0; i < n; ++i)
			{
				void *p = (void *)(&mPool[i]);
				new(p)T(mPool[i]);
				mFreeList.push_back((Dummy &)mPool[i]);
			}
		}

		~Pool()
		{
			_aligned_free(mPool);
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
