#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <Core/DataTypes.h>
#include <Core/Allocate.h>

#define OBJECT_POOL_PAGE_SIZE 1024

///////////////////////////////////////////////////////////////////////////////

namespace
{
	/* Page header (actually goes at end of page) */
	struct PageHeader
	{
		PageHeader() = default;
		PageHeader(void* start) :
			mNext		(0),
			mNextFree	((void**)start)
		{ }

		/* Pointer to next page */
		void* mNext;
		/* Free list */
		void** mNextFree;
	};
}

/* Paged pool allocator */
template <typename T>
class ObjectPool
{
public:
	ObjectPool() :
		mStart		(0),
		mCurrent	(0)
	{

	}

	~ObjectPool()
	{
		Free();
	}

	ObjectPool(const ObjectPool& other) = delete;
	ObjectPool& operator=(const ObjectPool& other) = delete;

	ObjectPool(ObjectPool&& other) :
		mStart		(0),
		mCurrent	(0)
	{
		mStart = other.mStart;
		mCurrent = other.mCurrent;

		other.mStart = 0;
		other.mCurrent = 0;
	}

	ObjectPool& operator=(ObjectPool&& other)
	{
		if (this != &other)
		{
			mStart = other.mStart;
			mCurrent = other.mCurrent;

			other.mStart = 0;
			other.mCurrent = 0;
		}

		return *this;
	}

	/* Free (all) memory */
	void Free()
	{
		T* ptr = mStart;
		while (ptr)
		{
			T* page = ptr;
			ptr = (T*)((PageHeader*)(page + OBJECT_POOL_PAGE_SIZE))->mNext;
			::Free(page);
		}
	}

	/* Create new object */
	T* New()
	{
		if (!mStart || !mCurrent)
		{
			mStart = AllocPage();
			mCurrent = mStart;
		}

		PageHeader* header = (PageHeader*)(mCurrent + OBJECT_POOL_PAGE_SIZE);
		
		if (!header->mNextFree)
		{
			// Allocate new block
			mStart = AllocPage();
			mCurrent = mStart;
		}

		// Next free stores pointer to slot location
		T* ptr = (T*)header->mNextFree;

		// Update next free
		header->mNextFree = (void**)(*header->mNextFree);

		// Initialize object
		new(ptr)T();

		return ptr;
	}

	/* Free object */
	void Free(T* ptr)
	{
		// Find which page pointer belongs to
		T* page = mStart;
		while (page)
		{
			// If pointer is inside block, then break from loop
			if (ptr >= page && ptr < page + OBJECT_POOL_PAGE_SIZE) break;

			// Go to next page
			page = (T*)((PageHeader*)(page + OBJECT_POOL_PAGE_SIZE))->mNext;
		}

		if (!page) return;

		// Call destructor
		ptr->~T();

		// Update free list
		PageHeader* header = (PageHeader*)(page + OBJECT_POOL_PAGE_SIZE);
		*(void**)ptr = (void*)header->mNextFree;
		header->mNextFree = (void**)ptr;
	}

private:
	/* Allocate page */
	T* AllocPage(Uint32 size = OBJECT_POOL_PAGE_SIZE)
	{
		T* ptr = (T*)Alloc(size * sizeof(T) + sizeof(PageHeader), alignof(T));

		// Initialize free list
		T* end = ptr + size;
		for (T* start = ptr; start < end; ++start)
			*(void**)start = start + 1;
		*(void**)(ptr + size - 1) = 0;

		*(PageHeader*)(ptr + size) = PageHeader(ptr);

		return ptr;
	}

private:
	/* Ptr to data */
	T* mStart;
	/* Current page */
	T* mCurrent;
};

///////////////////////////////////////////////////////////////////////////////

#endif