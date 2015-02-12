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

		void Release()
		{
			if(--mRefCount == 0)
			{
				delete this;
			}
		}

		int mRefCount;
	};
}