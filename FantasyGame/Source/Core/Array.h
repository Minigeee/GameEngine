#ifndef ARRAY_H
#define ARRAY_H

#include <Core/DataTypes.h>
#include <Core/Allocate.h>

#include <utility>

///////////////////////////////////////////////////////////////////////////////

/* Array of fixed size */
template <typename T>
class Array
{
public:
	/* Initialize array with no size */
	Array() :
		mStart		(0),
		mLast		(0),
		mEnd		(0)
	{

	}

	/* Initialize array with specified capacity */
	Array(Uint32 size) :
		mStart		(0),
		mLast		(0),
		mEnd		(0)
	{
		Resize(size);
	}

	~Array()
	{
		Free();
	}


	Array(const Array<T>& other) :
		mStart		(0),
		mLast		(0),
		mEnd		(0)
	{
		// Reserve own memory
		Resize(other.Capacity());

		// Copy all elements
		Uint32 size = other.Size();
		for (Uint32 i = 0; i < size; ++i)
			new(mStart + i)T(other.mStart[i]);

		mLast = mStart + size;
	}

	Array<T>& operator=(const Array<T>& other)
	{
		if (this != &other)
		{
			// Call element destructors
			Clear();

			if (other.Capacity() != Capacity())
				// Reserve own memory if capacity is different
				Resize(other.Capacity());

			// Copy all elements
			Uint32 size = other.Size();
			for (Uint32 i = 0; i < size; ++i)
				new(mStart + i)T(other.mStart[i]);

			mLast = mStart + size;
		}

		return *this;
	}

	Array(Array<T>&& other) :
		mStart		(0),
		mLast		(0),
		mEnd		(0)
	{
		mStart = other.mStart;
		mLast = other.mLast;
		mEnd = other.mEnd;

		other.mStart = 0;
		other.mLast = 0;
		other.mEnd = 0;
	}

	Array<T>& operator=(Array<T>&& other)
	{
		if (this != &other)
		{
			mStart = other.mStart;
			mLast = other.mLast;
			mEnd = other.mEnd;

			other.mStart = 0;
			other.mLast = 0;
			other.mEnd = 0;
		}

		return *this;
	}

	/* Access array element */
	T& operator[](Uint32 index) const
	{
		return mStart[index];
	}

	/* Access array element */
	T& At(Uint32 index) const
	{
		return mStart[index];
	}

	/* Get element at front */
	T& Front() const
	{
		return *mStart;
	}

	/* Get element at back */
	T& Back() const
	{
		return *(mLast - 1);
	}


	/* Push to back (copy) */
	void Push(const T& element)
	{
		if (mLast == mEnd)
			Resize(Capacity() * 2);

		new(mLast++)T(element);
	}

	/* Push to back (move) */
	void Push(T&& element)
	{
		if (mLast == mEnd)
			Resize(Capacity() * 2);

		new(mLast++)T(std::move(element));
	}

	/* Pop from back */
	void Pop()
	{
		(--mLast)->~T();
	}

	/* Remove element from array (Does not retain order) */
	void SwapPop(Uint32 index)
	{
		mStart[index].~T();
		new(mStart + index)T(std::move(*--mLast));
	}

	/* Remove element from array (Retains order) */
	void Remove(Uint32 index)
	{
		mStart[index].~T();

		// Shift all elements down
		Uint32 size = Size();
		for (Uint32 i = index; i < size - 1; ++i)
			new(mStart + i)T(std::move(mStart[i + 1]));

		--mLast;
	}


	/* Clear array */
	void Clear()
	{
		for (T* ptr = mStart; ptr < mLast; ++ptr)
			ptr->~T();

		mLast = mStart;
	}

	/* Resize array */
	void Resize(Uint32 size)
	{
		T* start = mStart;
		Uint32 prevSize = Size();

		if (Capacity() != size)
		{
			mStart = (T*)Alloc(size * sizeof(T), alignof(T));
			mLast = mStart + prevSize;
			mEnd = mStart + size;

			if (start)
			{
				// Copy previous data if it exists
				for (Uint32 i = 0; i < prevSize; ++i)
					new(mStart + i)T(std::move(*(start + i)));
			}

			// Free prev memory
			::Free(start);
		}
	}

	/* Free memory */
	void Free()
	{
		for (T* ptr = mStart; ptr < mLast; ++ptr)
			ptr->~T();

		// Free memory
		::Free(mStart);

		mStart = 0;
		mLast = 0;
		mEnd = 0;
	}


	/* Number of elements in array */
	Uint32 Size() const
	{
		return mLast - mStart;
	}

	/* Size of reserved memory (in number of elements) */
	Uint32 Capacity() const
	{
		return mEnd - mStart;
	}

	/* Returns if array is empty */
	bool IsEmpty() const
	{
		return mStart == mLast;
	}

private:
	/* Start of array */
	T* mStart;
	/* Last element in array */
	T* mLast;
	/* End of reserved memory */
	T* mEnd;
};

///////////////////////////////////////////////////////////////////////////////

#endif