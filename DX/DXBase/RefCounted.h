//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct RefCounted
	{
		RefCounted()
			: mRefCount(1)
		{
		}

		virtual ~RefCounted()
		{
		}

		void AddRef()
		{
			++mRefCount;
		}

		int Release()
		{
			int r = --mRefCount;
			if(r <= 0)
			{
				delete this;
			}
			return r;
		}

		int mRefCount;
	};
}