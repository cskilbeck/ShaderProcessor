//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	// Event - basically list of functions

	template<typename T> struct Event
	{
		// Delegate takes a reference to any kind of object (eg WindowEvent)

		template<typename T> using DelegateFunction = std::function<void(T const &event)>;

		// Base for linking handlers into lists

		struct Delegate: list_node<Delegate>
		{
		};

		// the handlers

		linked_list<Delegate> handlers;

		// typed event handler node, just a typed function pointer

		template<typename T> struct TypedDelegate: Delegate
		{
			DelegateFunction<T> handler;
		};

		~Event()
		{
			handlers.delete_all();
		}

		// add a function to the handler chain

		Delegate *AddListener(DelegateFunction<T> function)
		{
			TypedDelegate<T> *e = new TypedDelegate<T>();	// CRAP! Heap allocation...
			e->handler = function;
			handlers.push_back((Delegate *)e);
			return (Delegate *)e;
		}

		// remove a handler from the chain

		void Remove(Delegate *e)
		{
			handlers.remove(e);
			delete e;
		}

		// call all the handlers in the chain with the event args object

		void Fire(T const &event)
		{
			for(auto &e : handlers)
			{
				((TypedDelegate<T> &)e).handler(event);
			}
		}
	};
}