#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <Core/DataTypes.h>

///////////////////////////////////////////////////////////////////////////////

class ObjectPool
{
public:
	ObjectPool();
	ObjectPool(Uint32 size, Uint32 objSize, Uint32 align = 4);
	~ObjectPool();

	ObjectPool(const ObjectPool& other) = delete;
	ObjectPool& operator=(const ObjectPool& other) = delete;
	ObjectPool(ObjectPool&& other);
	ObjectPool& operator=(ObjectPool&& other);

	/* Create object pool */
	bool Create(Uint32 size, Uint32 objSize, Uint32 align = 4);
	/* Free (all) memory (Doesn't call destructors) */
	void Free();
	/* Allocate space for object */
	void* Alloc();
	/* Free space */
	void Free(void* ptr);

	/* Get data ptr */
	void* DataPtr() const;
	/* Get index of slot ptr */
	Uint32 GetIndex(void* ptr) const;

private:
	/* Ptr to data */
	Uint8* mStart;
	/* Number of slots */
	Uint32 mSize;
	/* Object size */
	Uint32 mObjSize;
	/* Free list */
	Uint32 mNextFree;
};

///////////////////////////////////////////////////////////////////////////////

#endif