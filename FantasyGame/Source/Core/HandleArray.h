#ifndef HANDLE_ARRAY_H
#define HANDLE_ARRAY_H

#include <Core/DataTypes.h>
#include <Core/Array.h>

#include <utility>

///////////////////////////////////////////////////////////////////////////////

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
	T& operator[](Uint32 handle) const
	{
		return mData[mHandleToIndex[handle]];
	}

	/* Add object and return handle */
	Uint32 Add(const T& object)
	{
		// If adding element will make array go over capacity, reserve extra space
		if (mData.Size() + 1 > mData.Capacity())
			Reserve(2 * mData.Capacity());

		Uint32 index = mData.Size();
		// Add object to data array
		mData.Push(object);

		// Store current next free entry index
		Uint32 handle = mNextFree;
		// Update next free index
		mNextFree = mHandleToIndex[mNextFree];
		// Map current free slot to added object
		mHandleToIndex[handle] = index;
		// Map data index to handle
		mIndexToHandle[index] = handle;

		return handle;
	}

	/* Add object and return handle */
	Uint32 Add(T&& object)
	{
		// If adding element will make array go over capacity, reserve extra space
		if (mData.Size() + 1 > mData.Capacity())
			Reserve(2 * mData.Capacity());

		Uint32 index = mData.Size();
		// Add object to data array
		mData.Push(std::move(object));

		// Store current next free entry index
		Uint32 handle = mNextFree;
		// Update next free index
		mNextFree = mHandleToIndex[mNextFree];
		// Map current free slot to added object
		mHandleToIndex[handle] = index;
		// Map data index to handle
		mIndexToHandle[index] = handle;

		return handle;
	}

	/* Remove object using handle */
	void Remove(Uint32 handle)
	{
		// Get index of object to remove
		Uint32 index = mHandleToIndex[handle];

		// Update free list
		mHandleToIndex[handle] = mNextFree;
		mNextFree = handle;

		// Pop swap to avoid moving all objects
		mData.SwapPop(index);

		// Update the mapping for object that was moved
		// mIndexToHandle[mData.Size()] --- Gets the handle of the object that used to be at the end of data array
		Uint16 movedHandle = mIndexToHandle[mData.Size()];
		// mHandleToIndex[movedHandle] = index --- Updates the current index for the handle whose object was moved
		mHandleToIndex[movedHandle] = index;

		// mIndexToHandle[index] = movedHandle --- Updates the handle that is in charge of the moved object's new position
		mIndexToHandle[index] = movedHandle;
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