//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	template<typename T> struct Delegate : list_node
	{
		template<typename T> using DelegateFunction = std::function<void(T const &event)>;

		Delegate(DelegateFunction<T> func = null)
			: mHandler(func)
			, mOwned(false)
		{
		}

		Delegate &operator = (DelegateFunction<T> function)
		{
			mHandler = function;
			return *this;
		}

		DelegateFunction<T> mHandler;
		bool mOwned;
	};

	//////////////////////////////////////////////////////////////////////

	template<typename T> struct Event
	{
		~Event()
		{
			while(!mHandlerList.empty())
			{
				Delegate<T> *h = mHandlerList.pop_front();
				if(h->mOwned)
				{
					delete h;
				}
			}
		}

		void operator += (Delegate<T> &d)
		{
			mHandlerList.push_back(d);
		}

		Delegate<T> *operator += (Delegate<T>::DelegateFunction<T> callback)
		{
			Delegate<T> *d = new Delegate<T>(callback);
			d->mOwned = true;
			mHandlerList.push_back(d);
			return d;
		}

		void operator -= (Delegate<T> &d)
		{
			mHandlerList.remove(d);
			if(d.mOwned)
			{
				// Huh? How did they get a pointer to the Delegate then?
				// from += dummy
			}
		}

		void Invoke(T const &event)
		{
			for(auto &e : mHandlerList)
			{
				e.mHandler(event);
			}
		}

		chs::linked_list<Delegate<T>> mHandlerList;
	};
}