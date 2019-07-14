#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <Core/DataTypes.h>
#include <Core/Allocate.h>

#include <vector>

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
		mCurrent	(0),
		mPageSize	(1024)
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
			if (mStart)
				Free();

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
		std::vector<bool> filled(mPageSize, true);

		T* ptr = mStart;
		while (ptr)
		{
			T* page = ptr;
			PageHeader* header = (PageHeader*)(page + mPageSize);
			ptr = (T*)header->mNext;

			// Mark which slots were not used
			T** nextFree = (T**)header->mNextFree;
			while (nextFree)
			{
				filled[(T*)nextFree - page] = false;
				nextFree = (T**)(*nextFree);
			}

			// Call destructors on ones that are being used
			for (Uint32 i = 0; i < mPageSize; ++i)
			{
				if (filled[i])
					(page + i)->~T();
				else
					filled[i] = true;
			}

			::Free(page);
		}
	}

	/* Create new object */
	T* New()
	{
		if (!mStart || !mCurrent)
		{
			mStart = AllocPage(mPageSize);
			mCurrent = mStart;
		}

		PageHeader* header = (PageHeader*)(mCurrent + mPageSize);
		
		if (!header->mNextFree)
		{
			// Allocate new block
			mStart = AllocPage(mPageSize);
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
			if (ptr >= page && ptr < page + mPageSize) break;

			// Go to next page
			page = (T*)((PageHeader*)(page + mPageSize))->mNext;
		}

		if (!page) return;

		// Call destructor
		ptr->~T();

		// Update free list
		PageHeader* header = (PageHeader*)(page + mPageSize);
		*(void**)ptr = (void*)header->mNextFree;
		header->mNextFree = (void**)ptr;
	}

	/* Set page size if nothing has been allocated yet */
	void SetPageSize(Uint32 size)
	{
		mPageSize = size;
	}

private:
	/* Allocate page */
	T* AllocPage(Uint32 size)
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
	/* Page size */
	Uint32 mPageSize;
};

///////////////////////////////////////////////////////////////////////////////

#endif