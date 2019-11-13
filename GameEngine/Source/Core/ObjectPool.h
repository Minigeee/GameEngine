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
		mLast		(0),
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
		mLast = other.mLast;
		mPageSize = other.mPageSize;

		other.mStart = 0;
		other.mCurrent = 0;
		other.mLast = 0;
		other.mPageSize = 1024;
	}

	ObjectPool& operator=(ObjectPool&& other)
	{
		if (this != &other)
		{
			if (mStart)
				Free();

			mStart = other.mStart;
			mCurrent = other.mCurrent;
			mLast = other.mLast;
			mPageSize = other.mPageSize;

			other.mStart = 0;
			other.mCurrent = 0;
			other.mLast = 0;
			other.mPageSize = 1024;
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

		mStart = 0;
		mCurrent = 0;
		mLast = 0;
	}

	/* Create new object */
	T* New()
	{
		if (!mStart || !mCurrent)
		{
			mStart = AllocPage(mPageSize);
			mCurrent = mStart;
			mLast = mCurrent;
		}

		// Get header of current page
		PageHeader* header = (PageHeader*)(mCurrent + mPageSize);
		
		// If this page is filled up...
		if (!header->mNextFree)
		{
			// Find first page with free slots
			T* page = mStart;
			// While current page exists...
			while (page)
			{
				header = (PageHeader*)(page + mPageSize);
				// If this page has open slots, break loop
				if (header->mNextFree) break;

				// Go to next page
				page = (T*)header->mNext;
			}

			// If a page with empty slots was found...
			if (page)
			{
				// Make this page the current
				mCurrent = page;
			}

			// Otherwise...
			else
			{
				// Allocate new block
				mCurrent = AllocPage(mPageSize);
				// Make header of the last page point to new page
				header = (PageHeader*)(mLast + mPageSize);
				header->mNext = mCurrent;

				// Update header
				header = (PageHeader*)(mCurrent + mPageSize);
				// Update last page
				mLast = mCurrent;
			}
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
	/* Last page */
	T* mLast;
	/* Page size */
	Uint32 mPageSize;
};

///////////////////////////////////////////////////////////////////////////////

#endif