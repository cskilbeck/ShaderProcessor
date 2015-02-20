//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	template<typename T> struct Delegate
	{
		template<typename T> using DelegateFunction = std::function<void(T const &event)>;

		Delegate(DelegateFunction<T> func = null)
			: handler(func)
			, owned(false)
		{
		}

		Delegate &operator = (DelegateFunction<T> function)
		{
			handler = function;
			return *this;
		}

		list_node<Delegate<T>> mListNode;
		DelegateFunction<T> handler;
		bool owned;
	};

	//////////////////////////////////////////////////////////////////////

	template<typename T> struct Event
	{
		~Event()
		{
			while(!handlers.empty())
			{
				Delegate<T> *h = handlers.pop_front();
				if(h->owned)
				{
					delete h;
				}
			}
		}

		void operator += (Delegate<T> &d)
		{
			handlers.push_back(d);
		}

		void operator += (Delegate<T>::DelegateFunction<T> callback)
		{
			Delegate<T> *d = new Delegate<T>(callback);
			d->owned = true;
			handlers.push_back(d);
		}

		void operator -= (Delegate<T> &d)
		{
			handlers.remove(d);
			if(d->owned)
			{
				// Huh?
			}
		}

		void Invoke(T const &event)
		{
			for(auto &e : handlers)
			{
				((Delegate<T> &)e).handler(event);
			}
		}

		linked_list<Delegate<T>, &Delegate<T>::mListNode> handlers;
	};
}