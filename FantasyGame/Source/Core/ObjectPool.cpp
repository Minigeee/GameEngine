#include <Core/ObjectPool.h>
#include <Core/Allocate.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ObjectPool::ObjectPool() :
	mStart		(0),
	mSize		(0),
	mObjSize	(0),
	mNextFree	(0)
{

}

ObjectPool::ObjectPool(Uint32 size, Uint32 objSize, Uint32 align) :
	mStart		(0),
	mSize		(0),
	mObjSize	(0),
	mNextFree	(0)
{
	Create(size, objSize, align);
}

ObjectPool::~ObjectPool()
{
	Free();
}

///////////////////////////////////////////////////////////////////////////////

ObjectPool::ObjectPool(ObjectPool&& other) :
	mStart		(0),
	mSize		(0),
	mObjSize	(0),
	mNextFree	(0)
{
	mStart = other.mStart;
	mSize = other.mSize;
	mObjSize = other.mObjSize;
	mNextFree = other.mNextFree;

	other.mStart = 0;
	other.mSize = 0;
	other.mObjSize = 0;
	other.mNextFree = 0;
}

ObjectPool& ObjectPool::operator=(ObjectPool&& other)
{
	if (this != &other)
	{
		mStart = other.mStart;
		mSize = other.mSize;
		mObjSize = other.mObjSize;
		mNextFree = other.mNextFree;

		other.mStart = 0;
		other.mSize = 0;
		other.mObjSize = 0;
		other.mNextFree = 0;
	}

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool ObjectPool::Create(Uint32 size, Uint32 objSize, Uint32 align)
{
	mStart = (Uint8*)::Alloc(size * objSize, align);
	mSize = size;
	mObjSize = objSize;

	if (!mStart)
		return false;

	// Setup free list
	for (Uint32 i = 0; i < size; ++i)
		*(Uint32*)(mStart + i * objSize) = i + 1;

	return true;
}

///////////////////////////////////////////////////////////////////////////////

void ObjectPool::Free()
{
	::Free(mStart);

	mStart = 0;
	mSize = 0;
	mObjSize = 0;
	mNextFree = 0;
}

///////////////////////////////////////////////////////////////////////////////

void* ObjectPool::Alloc()
{
	// If no more spots, return 0
	if (mNextFree >= mSize)
		return 0;

	Uint8* ptr = mStart + mNextFree * mObjSize;

	// Update free list
	mNextFree = *(Uint32*)ptr;

	return (void*)ptr;
}

void ObjectPool::Free(void* ptr)
{
	Uint32 offset = ((Uint8*)ptr - mStart) / mObjSize;
	// Only free if data is inside allocated block
	if (offset >= mSize) return;

	// Update free list
	*(Uint32*)ptr = mNextFree;
	mNextFree = offset;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void* ObjectPool::DataPtr() const
{
	return (void*)mStart;
}

///////////////////////////////////////////////////////////////////////////////

Uint32 ObjectPool::GetIndex(void* ptr) const
{
	Uint32 offset = (Uint8*)ptr - mStart;
	return offset / mObjSize;
}

///////////////////////////////////////////////////////////////////////////////