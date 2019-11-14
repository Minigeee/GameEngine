#ifndef HANDLE_ARRAY_H
#define HANDLE_ARRAY_H

#include <Core/DataTypes.h>
#include <Core/Array.h>

#include <utility>

///////////////////////////////////////////////////////////////////////////////

typedef Uint16 Handle;

template <typename T>
class HandleArray
{
public:
	HandleArray() :
		mNextFree		(0)
	{

	}

	HandleArray(Uint32 size) :
		mNextFree		(0)
	{
		Reserve(size);
	}

	/* Get object using handle */
	T& operator[](Handle handle) const
	{
		return mData[mHandleToIndex[handle]];
	}

	/* Add object and return handle */
	Handle Add(const T& object)
	{
		// If adding element will make array go over capacity, reserve extra space
		if (mData.Size() + 1 > mData.Capacity())
			Reserve(2 * mData.Capacity());

		Uint32 index = mData.Size();
		// Add object to data array
		mData.Push(object);

		// Store current next free entry index
		Handle handle = (Handle)mNextFree;
		// Update next free index
		mNextFree = mHandleToIndex[mNextFree];
		// Map current free slot to added object
		mHandleToIndex[handle] = index;
		// Map data index to handle
		mIndexToHandle[index] = handle;

		return handle;
	}

	/* Add object and return handle */
	Handle Add(T&& object)
	{
		// If adding element will make array go over capacity, reserve extra space
		if (mData.Size() + 1 > mData.Capacity())
			Reserve(2 * mData.Capacity());

		Uint32 index = mData.Size();
		// Add object to data array
		mData.Push(std::move(object));

		// Store current next free entry index
		Handle handle = (Handle)mNextFree;
		// Update next free index
		mNextFree = mHandleToIndex[handle];
		// Map current free slot to added object
		mHandleToIndex[handle] = index;
		// Map data index to handle
		mIndexToHandle[index] = handle;

		return handle;
	}

	/* Remove object using handle */
	void Remove(Handle handle)
	{
		// Get index of the item that is being removed
		Uint32 targetIndex = mHandleToIndex[handle];

		// Remove item using a swap pop to avoid item shifting
		mData.SwapPop(targetIndex);

		// Find the handle of the item that was moved from the end to fill the item that was just removed
		Handle movedHandle = mIndexToHandle[mData.Size()];

		// Map the moved item's handle to its new index position
		mHandleToIndex[movedHandle] = targetIndex;

		// Map the index position of the moved item to its handle
		mIndexToHandle[targetIndex] = movedHandle;

		// Store next free handle in the handle position of the item that was removed
		mHandleToIndex[handle] = mNextFree;

		// Mark the handle that was removed as the next free
		mNextFree = handle;
	}

	/* Reserve space for handle array */
	void Reserve(Uint32 size)
	{
		if (size < mHandleToIndex.Capacity())
		{
			// If size is smaller, clear all data
			mData.Clear();
			mHandleToIndex.Clear();
			mIndexToHandle.Clear();
		}

		// The end of free list will always point to one past the end of previous capacity,
		// So start creating the free list from that index
		Uint32 prevCap = mHandleToIndex.Capacity();

		// Reserve data
		mData.Reserve(size);

		// Reserve mappings
		mHandleToIndex.Reserve(size);
		mIndexToHandle.Reserve(size);

		// Create free list
		for (Uint32 i = prevCap; i < mHandleToIndex.Capacity(); ++i)
			mHandleToIndex.Push(i + 1);
	}

	/* Number of elements in array */
	Uint32 Size() const
	{
		return mData.Size();
	}

	/* Size of reserved memory (in number of elements) */
	Uint32 Capacity() const
	{
		return mData.Capacity();
	}

	/* Returns if array is empty */
	bool IsEmpty() const
	{
		return mData.IsEmpty();
	}

	/* Returns the linear data array */
	Array<T>& GetData()
	{
		return mData;
	}


	/* Map handle to internal index */
	Uint32 HandleToIndex(Handle handle)
	{
		return mHandleToIndex[handle];
	}

	/* Map internal index to handle */
	Handle IndexToHandle(Uint32 index)
	{
		return (Handle)mIndexToHandle[index];
	}

private:
	/* Array for objects */
	Array<T> mData;
	/* Map handles to indices */
	Array<Uint16> mHandleToIndex;
	/* Map indices to handles */
	Array<Uint16> mIndexToHandle;
	/* Index of next free slot */
	Uint32 mNextFree;
};

///////////////////////////////////////////////////////////////////////////////

#endif