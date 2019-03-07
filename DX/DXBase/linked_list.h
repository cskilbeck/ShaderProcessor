//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////
// Deal with MSVC internal compiler error, this can be removed when a fix is available

#pragma push_macro("NODE_IDENT")
#define NODE_IDENT (list_node T::*)nullptr != NODE

#include <functional>

//////////////////////////////////////////////////////////////////////

namespace chs
{

	//////////////////////////////////////////////////////////////////////
	// base list node class, 2 pointers

	struct list_node_base
	{
		void *next;
		void *prev;
	};

	//////////////////////////////////////////////////////////////////////
	// base node is wrapped so we can get the offset to it

	struct list_node
	{
		list_node_base node;
	};

	//////////////////////////////////////////////////////////////////////
	// template base

	template <typename T, list_node T::*NODE, bool is_member> class list_base
	{
	};

	//////////////////////////////////////////////////////////////////////
	// specialization for instances using list_node as member field

	template <typename T, list_node T::*NODE> class list_base<T, NODE, true> : protected list_node
	{
	protected:
		static size_t offset()
		{
			list_node *b = &(((T *)0)->*NODE);
			return size_t(&b->node);
		}

		typedef list_base<T, NODE, true> list_type;
	};

	//////////////////////////////////////////////////////////////////////
	// specialization for instances deriving from list_node

	template <typename T, list_node T::*NODE> class list_base<T, NODE, false> : protected list_node
	{
		// this static_assert can't work if you want this:
		//
		// struct X : list_node
		// {
		//     linked_list<X> l;	// static assert throws incomplete type error...
		// };
		
		// static_assert(!std::is_polymorphic<T>::value, "polymorphic! use the member-node version");

		// it's not the end of the world because you'll get a compiler error
		// if the type is truly polymorphic but it will be very cryptic

	protected:
		static size_t offset()
		{
			list_node_base T::*n = static_cast<list_node_base T::*>(&T::node);
			return (size_t)(&(((T *)0)->*n));
		}
		typedef list_base<T, NODE, false> list_type;
	};

	//////////////////////////////////////////////////////////////////////

	template <typename T, list_node T::*NODE = nullptr> class linked_list: protected list_base < T, NODE, NODE_IDENT >
	{
	public:

		//////////////////////////////////////////////////////////////////////

		using list_base<T, NODE, NODE_IDENT>::offset;
		using list_base<T, NODE, NODE_IDENT>::node;

		//////////////////////////////////////////////////////////////////////

		typedef linked_list<T, NODE> list_t;

		typedef T *          ptr;
		typedef T *          pointer;
		typedef T const *    const_ptr;
		typedef T const *    const_pointer;
		typedef T &          ref;
		typedef T &          reference;
		typedef T const &    const_ref;
		typedef T const &    const_reference;

		//////////////////////////////////////////////////////////////////////

#if !defined(_CHS_LINKED_LIST_DONT_DEFINE_STL_ITERATORS_)

		typedef std::iterator<std::bidirectional_iterator_tag, T> iterator_base;

		//////////////////////////////////////////////////////////////////////

		struct const_iterator: iterator_base
		{
			const_iterator(const_ptr t) : p(t)
			{
			}
			const_iterator(const_iterator const &o) : p(o.p)
			{
			}
			const_iterator const &operator=(const_iterator const &o)
			{
				p = o.p;
				return *this;
			}
			const_iterator &operator++()
			{
				p = (const_ptr)get_node(p).next;
				return *this;
			}
			const_iterator &operator--()
			{
				p = (const_ptr)get_node(p).prev;
				return *this;
			}
			bool operator==(const_iterator const &o)
			{
				return p == o.p;
			}
			bool operator!=(const_iterator const &o)
			{
				return p != o.p;
			}
			const_ref operator *()
			{
				return *p;
			}
			const_ptr operator->()
			{
				return  p;
			}
			operator ptr()
			{
				return p;
			}
		private:
			const_ptr p;
		};

		//////////////////////////////////////////////////////////////////////

		struct iterator: iterator_base
		{
			iterator(ptr t) : p(t)
			{
			}
			iterator(const_ptr *t) : p(t)
			{
			}
			iterator(iterator const &o) : p(o.p)
			{
			}
			iterator const &operator=(const_iterator const &o)
			{
				p = o.p;
				return *this;
			}
			iterator const &operator=(iterator const &o)
			{
				p = o.p;
				return *this;
			}
			iterator &operator++()
			{
				p = (ptr)get_node(p).next;
				return *this;
			}
			iterator &operator--()
			{
				p = (ptr)get_node(p).prev;
				return *this;
			}
			bool operator==(iterator const &o)
			{
				return p == o.p;
			}
			bool operator!=(iterator const &o)
			{
				return p != o.p;
			}
			ref operator *()
			{
				return *p;
			}
			ptr operator->()
			{
				return  p;
			}
			operator ptr()
			{
				return p;
			}
		private:
			ptr p;
		};

		//////////////////////////////////////////////////////////////////////

		struct const_reverse_iterator: iterator_base
		{
			const_reverse_iterator(const_ptr t) : p(t)
			{
			}
			const_reverse_iterator(const_reverse_iterator const &o) : p(o.p)
			{
			}
			const_reverse_iterator const &operator=(const_reverse_iterator const &o)
			{
				p = o.p;
				return o;
			}
			const_reverse_iterator &operator++()
			{
				p = (const_ptr)get_node(p).prev;
				return *this;
			}
			const_reverse_iterator &operator--()
			{
				p = (const_ptr)get_node(p).next;
				return *this;
			}
			bool operator==(const_reverse_iterator const &o)
			{
				return p == o.p;
			}
			bool operator!=(const_reverse_iterator const &o)
			{
				return p != o.p;
			}
			const_ref operator *()
			{
				return *p;
			}
			const_ptr operator->()
			{
				return  p;
			}
			operator ptr()
			{
				return p;
			}
		private:
			const_ptr p;
		};

		//////////////////////////////////////////////////////////////////////

		struct reverse_iterator: iterator_base
		{
			reverse_iterator(ptr t) : p(t)
			{
			}
			reverse_iterator(const_ptr t) : p(t)
			{
			}
			reverse_iterator(reverse_iterator const &o) : p(o.p)
			{
			}
			reverse_iterator const &operator=(const_reverse_iterator const &o)
			{
				p = o.p;
				return *this;
			}
			reverse_iterator const &operator=(reverse_iterator const &o)
			{
				p = o.p;
				return *this;
			}
			reverse_iterator &operator++()
			{
				p = (ptr)get_node(p).prev;
				return *this;
			}
			reverse_iterator &operator--()
			{
				p = (ptr)get_node(p).next;
				return *this;
			}
			bool operator==(reverse_iterator const &o)
			{
				return p == o.p;
			}
			bool operator!=(reverse_iterator const &o)
			{
				return p != o.p;
			}
			ref operator *()
			{
				return *p;
			}
			ptr operator->()
			{
				return  p;
			}
			operator ptr()
			{
				return p;
			}
		private:
			ptr p;
		};

		//////////////////////////////////////////////////////////////////////

		iterator begin()
		{
			return iterator((ptr)node.next);
		}
		const_iterator begin() const
		{
			return const_iterator((const_ptr)node.next);
		}

		reverse_iterator rbegin()
		{
			return reverse_iterator((ptr)node.prev);
		}
		const_reverse_iterator rbegin() const
		{
			return const_reverse_iterator((const_ptr)node.prev);
		}

		iterator end()
		{
			return iterator(root());
		}
		const_iterator end() const
		{
			return const_iterator(const_root());
		}

		reverse_iterator rend()
		{
			return reverse_iterator(root());
		}
		const_reverse_iterator rend() const
		{
			return const_reverse_iterator(const_root());
		}

		const_iterator cbegin() const
		{
			return const_iterator(node.next);
		}
		const_iterator cend() const
		{
			return const_iterator(const_root());
		}

		const_reverse_iterator crbegin() const
		{
			return const_reverse_iterator(node.prev);
		}
		const_reverse_iterator crend() const
		{
			return const_reverse_iterator(const_root());
		}

#endif //!defined(_CHS_LINKED_LIST_DONT_DEFINE_STL_ITERATORS_)

		//////////////////////////////////////////////////////////////////////

		typedef list_node_base	         node_t;
		typedef node_t *                 node_ptr;
		typedef node_t &                 node_ref;
		typedef list_node_base    const  const_node_t;
		typedef const_node_t *           const_node_ptr;
		typedef const_node_t &           const_node_ref;

	private:

		//////////////////////////////////////////////////////////////////////

		static list_t &transfer(list_t &a, list_t &b)
		{
			if(!a.empty())
			{
				ptr ot = a.tail();
				ptr oh = a.head();
				ptr rt = b.root();
				set_next(ot, rt);
				set_prev(oh, rt);
				set_prev(rt, ot);
				set_next(rt, oh);
				a.clear();
			}
			else
			{
				b.clear();
			}
			return b;
		}

		//////////////////////////////////////////////////////////////////////

		static ptr get_object(node_ptr node)
		{
			return (ptr)((char *)node - offset());
		}

		//////////////////////////////////////////////////////////////////////

		static node_ref get_node(ptr obj)
		{
			return *((node_ptr)((char *)obj + offset()));
		}

		//////////////////////////////////////////////////////////////////////

		static const_node_ref get_node(const_ptr obj)
		{
			return *((const_node_ptr)((char const *)obj + offset()));
		}

		//////////////////////////////////////////////////////////////////////

		ptr root()
		{
			return (ptr)((char *)&node - offset());
		}

		//////////////////////////////////////////////////////////////////////

		const_ptr const_root() const
		{
			return (const_ptr)((char const *)&node - offset());
		}

		//////////////////////////////////////////////////////////////////////

		static ptr get_next(ptr const o)
		{
			return (ptr)get_node(o).next;
		}

		//////////////////////////////////////////////////////////////////////

		static ptr get_prev(ptr const o)
		{
			return (ptr)get_node(o).prev;
		}

		//////////////////////////////////////////////////////////////////////

		static void set_next(ptr o, const_ptr p)
		{
			get_node(o).next = (void *)p;
		}

		//////////////////////////////////////////////////////////////////////

		static void set_prev(ptr o, const_ptr p)
		{
			get_node(o).prev = (void *)p;
		}

	public:

		//////////////////////////////////////////////////////////////////////

		void clear()
		{
			node.next = root();
			node.prev = root();
		}

		//////////////////////////////////////////////////////////////////////

		void insert_before(ptr obj_before, ptr obj)
		{
			ptr &p = (ptr &)get_node(obj_before).prev;
			node_ref n = get_node(obj);
			get_node(p).next = obj;
			n.prev = (void *)p;
			n.next = (void *)obj_before;
			p = obj;
		}

		//////////////////////////////////////////////////////////////////////

		void insert_after(ptr obj_after, ptr obj)
		{
			ptr &n = (ptr &)get_node(obj_after).next;
			node_ref p = get_node(obj);
			get_node(n).prev = obj;
			p.next = (void *)n;
			p.prev = (void *)obj_after;
			n = obj;
		}

		//////////////////////////////////////////////////////////////////////

		ptr remove(ptr obj)
		{
			ptr p = prev(obj);
			ptr n = next(obj);
			set_next(p, n);
			set_prev(n, p);
			return obj;
		}

		//////////////////////////////////////////////////////////////////////

		void remove_range(ptr f, ptr l)
		{
			ptr op = prev(f);
			ptr on = next(l);
			set_next(op, on);
			set_prev(on, op);
		}

		//////////////////////////////////////////////////////////////////////

		void move_range_before(ptr where, list_t &other, ptr f, ptr l)
		{
			other.remove_range(f, l);
			ptr p = get_node(where).prev;
			set_next(p, f);
			set_prev(f, p);
			set_prev(where, l);
			set_next(l, where);
		}

		//////////////////////////////////////////////////////////////////////

		void move_range_after(ptr where, list_t &other, ptr f, ptr l)
		{
			other.remove_range(f, l);
			ptr p = get_node(where).next;
			set_prev(p, l);
			set_next(l, p);
			set_next(where, f);
			set_prev(f, where);
		}

		//////////////////////////////////////////////////////////////////////

		linked_list()
		{
			clear();
		}

		//////////////////////////////////////////////////////////////////////

		explicit linked_list(list_t &other)
		{
			transfer(other, *this);
		}

		//////////////////////////////////////////////////////////////////////

		list_t &operator = (list_t &o)
		{
			return transfer(o, *this);
		}

		//////////////////////////////////////////////////////////////////////

		list_t const &operator = (list_t const &&o)
		{
			static_assert(false, "no assignments please...");
			return &nullptr;
		}

		//////////////////////////////////////////////////////////////////////

		list_t &operator = (list_t &&o)
		{
			static_assert(false, "no rvalue moves please...");
			return &nullptr;
		}

		//////////////////////////////////////////////////////////////////////

		bool empty() const
		{
			return node.next == const_root();
		}

		//////////////////////////////////////////////////////////////////////

		size_t size() const
		{
			size_t count = 0;
			for(auto const &i : *this)
			{
				++count;
			}
			return count;
		}

		//////////////////////////////////////////////////////////////////////

		ptr head()
		{
			return (ptr)node.next;
		}

		ptr tail()
		{
			return (ptr)node.prev;
		}

		const_ptr c_head() const
		{
			return (const_ptr)node.next;
		}

		const_ptr c_tail() const
		{
			return (const_ptr)node.prev;
		}

		ptr next(ptr obj)
		{
			return (ptr)get_node(obj).next;
		}

		ptr prev(ptr obj)
		{
			return (ptr)get_node(obj).prev;
		}

		const_ptr c_next(const_ptr obj) const
		{
			return (const_ptr)get_node(obj).next;
		}

		const_ptr c_prev(const_ptr obj) const
		{
			return (const_ptr)get_node(obj).prev;
		}

		ptr done()
		{
			return root();
		}

		const_ptr c_done() const
		{
			return const_root();
		}

		//////////////////////////////////////////////////////////////////////

		ptr remove(ref obj)
		{
			return remove(&obj);
		}

		void push_back(ref obj)
		{
			insert_before(*root(), obj);
		}

		void push_back(ptr obj)
		{
			insert_before(root(), obj);
		}

		void push_front(ref obj)
		{
			insert_after(*root(), obj);
		}

		void push_front(ptr obj)
		{
			insert_after(root(), obj);
		}

		ptr pop_front()
		{
			return empty() ? nullptr : remove(head());
		}

		ptr pop_back()
		{
			return empty() ? nullptr : remove(tail());
		}

		void insert_before(ref bef, ref obj)
		{
			insert_before(&bef, &obj);
		}

		void insert_after(ref aft, ref obj)
		{
			insert_after(&aft, &obj);
		}

		void remove_range(ref f, ref l)
		{
			remove_range(&f, &l);
		}

		void move_range_before(ref where, list_t &other, ref f, ref l)
		{
			move_range_before(&where, other, &f, &l);
		}

		void move_range_after(ref where, list_t &other, ref f, ref l)
		{
			move_range_after(&where, other, &f, &l);
		}

		//////////////////////////////////////////////////////////////////////

		void append(list_t &other_list)
		{
			if(!other_list.empty())
			{
				ptr oh = other_list.head();
				ptr ot = other_list.tail();
				ptr rt = root();
				ptr mt = tail();
				get_node(mt).next = oh;
				get_node(oh).prev = mt;
				get_node(ot).next = rt;
				get_node(rt).prev = ot;
				other_list.clear();
			}
		}

		//////////////////////////////////////////////////////////////////////

		void prepend(list_t &other_list)
		{
			if(!other_list.empty())
			{
				ptr oh = other_list.head();
				ptr ot = other_list.tail();
				ptr rt = root();
				ptr mh = head();
				get_node(mh).prev = ot;
				get_node(ot).next = mh;
				get_node(oh).prev = rt;
				get_node(rt).next = oh;
				other_list.clear();
			}
		}

		//////////////////////////////////////////////////////////////////////

		template<typename N> void copy_into(N &n)
		{
			static_assert(!std::is_same<N, list_t>::value, "can't copy to the same type!");
			for(auto &p : *this)
			{
				n.push_back(p);
			}
		}

		//////////////////////////////////////////////////////////////////////

		void split(ptr obj, list_t &new_list)
		{
			T *prev_obj = prev(obj);
			if(prev_obj == root())
			{
				transfer(*this, new_list);
			}
			else
			{
				T *new_root = new_list.root();
				T *old_tail = tail();
				T *next_obj = next(obj);
				if(next_obj == root())
				{
					get_node(old_tail).prev = new_root;
				}
				get_node(old_tail).next = new_root;
				get_node(prev_obj).next = root();
				get_node(root()).prev = prev_obj;
				new_list.get_node(new_root).next = obj;
				new_list.get_node(new_root).prev = old_tail;
			}
		}

		//////////////////////////////////////////////////////////////////////

	private:

		static void merge(list_t &left, list_t &right, std::function<bool(T const &a, T const &b)> const &compare)
		{
			ptr insert_point = right.root();
			ptr run_head = left.head();
			ptr ad = left.done();
			ptr bd = right.done();
			while(run_head != ad)
			{
				// find where to put a run
				do
				{
					insert_point = get_next(insert_point);
				}
				while(insert_point != bd && !compare(*run_head, *insert_point));

				// scanned off the end?
				if(insert_point != bd)
				{
					// no, find how long the run should be
					ptr run_start = run_head;
					ptr run_end;
					do
					{
						run_end = run_head;
						run_head = get_next(run_head);
					}
					while(run_head != ad && !compare(*insert_point, *run_head));

					// insert it
					ptr p = get_prev(insert_point);
					set_prev(run_start, p);
					set_next(p, run_start);
					set_prev(insert_point, run_end);
					set_next(run_end, insert_point);
				}
				else
				{   // yes, append remainder
					ptr ot = left.tail();
					ptr rt = right.root();
					ptr mt = right.tail();
					set_prev(run_head, mt);
					set_next(mt, run_head);
					set_prev(rt, ot);
					set_next(ot, rt);
					break;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////
		// thanks to the putty guy

		static void merge_sort(list_t &list, size_t size, std::function<bool(T const &a, T const &b)> const &compare)
		{
			if(size > 2)
			{
				// scan for midpoint
				size_t left_size = size / 2;
				size_t right_size = size - left_size;
				ptr pm = list.head();
				for(size_t s = 0; s < left_size; ++s)
				{
					pm = list.next(pm);
				}

				// split into left, right
				list_t left;
				list_t right;
				ptr lr = left.root();
				ptr rr = right.root();
				ptr ot = list.tail();
				ptr oh = list.head();
				ptr pp = list.get_prev(pm);
				set_prev(lr, pp);
				set_next(lr, oh);
				set_prev(oh, lr);
				set_next(pp, lr);
				set_prev(rr, ot);
				set_next(rr, pm);
				set_prev(pm, rr);
				set_next(ot, rr);

				// sort them
				merge_sort(left, left_size, compare);
				merge_sort(right, right_size, compare);

				// stitch them back together
				merge(right, left, compare);

				// move result back into list
				ot = left.tail();
				oh = left.head();
				lr = list.root();
				set_prev(oh, lr);
				set_next(ot, lr);
				set_prev(lr, ot);
				set_next(lr, oh);
			}
			else if(size > 1)
			{
				// dinky list of 2 entries, just fix it
				ptr h = list.head();
				ptr t = list.tail();
				if(compare(*t, *h))
				{
					ptr r = list.root();
					set_next(r, t);
					set_prev(r, h);
					set_next(h, r);
					set_prev(h, t);
					set_next(t, h);
					set_prev(t, r);
				}
			}
		}

		//////////////////////////////////////////////////////////////////////

	public:

		void sort()
		{
			auto less = [] (T const &a, T const &b)
			{
				return a < b;
			};
			merge_sort(*this, size(), less);
		}

		//////////////////////////////////////////////////////////////////////

		void sort(std::function<bool(T const &a, T const &b)> const &compare)
		{
			merge_sort(*this, size(), compare);
		}

		//////////////////////////////////////////////////////////////////////

		void merge_into(list_t &other)
		{
			merge(*this, other);
			clear();
		}

		//////////////////////////////////////////////////////////////////////

		void for_each(std::function<void(T &)> func)
		{
			for(auto i = begin(), _end = end(), n = i; ++n, i != _end; i = n)
			{
				func(*i);
			}
		}

		//////////////////////////////////////////////////////////////////////

		list_t select(std::function<bool(T &)> func)
		{
			list_t t;
			for(auto i = begin(), _end = end(), n = i; ++n, i != _end; i = n)
			{
				if(func(*i))
				{
					t.push_back(remove(i));
				}
			}
			return t;
		}

		//////////////////////////////////////////////////////////////////////

		template<class O> ptr find_first_of(O const &t)
		{
			for(auto p = head(); p != done(); p = next(p))
			{
				if(*p == t)
				{
					return p;
				}
			}
			return nullptr;
		}

		//////////////////////////////////////////////////////////////////////

		template<typename O> ptr find_last_of(O const &t)
		{
			for(auto p = tail(); p != done(); p = prev(p))
			{
				if(*p == t)
				{
					return p;
				}
			}
			return nullptr;
		}

		//////////////////////////////////////////////////////////////////////

		unsigned remove_if(std::function<bool(ref)> func)
		{
			unsigned total = 0;
			for(auto i = begin(), _end = end(), n = i; ++n, i != _end; i = n)
			{
				if(func(*i))
				{
					remove(i);
					++total;
				}
			}
			return total;
		}

		//////////////////////////////////////////////////////////////////////

		unsigned delete_if(std::function<bool(ref)> func)
		{
			unsigned total = 0;
			for(auto i = begin(), _end = end(), n = i; ++n, i != _end; i = n)
			{
				if(func(*i))
				{
					remove(i);
					delete i;
					++total;
				}
			}
			return total;
		}

		//////////////////////////////////////////////////////////////////////

		void remove_all()
		{
			clear();
		}

		//////////////////////////////////////////////////////////////////////

		void delete_all()
		{
			while(!empty())
			{
				ptr i = pop_front();
				delete i;
			}
		}
	};

	//////////////////////////////////////////////////////////////////////

} // chs

#pragma warning(push)
#pragma warning(disable: 4602)        // disable spurious warning

#pragma pop_macro("NODE_IDENT")
#pragma warning(pop)

