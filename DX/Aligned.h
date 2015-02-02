//////////////////////////////////////////////////////////////////////

struct Aligned16
{
	void *operator new(size_t s)
	{
		return _aligned_malloc(s, 16);
	}

	void operator delete(void *p)
	{
		_aligned_free(p);
	}

	void *operator new[] (size_t s)
	{
		return _aligned_malloc(s, 16);
	}

	void operator delete[] (void *p)
	{
		_aligned_free(p);
	}
};
