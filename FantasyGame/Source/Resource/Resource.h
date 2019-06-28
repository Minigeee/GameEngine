#ifndef RESOURCE_H
#define RESOURCE_H

#include <Core/DataTypes.h>
#include <Core/ObjectPool.h>
#include <Core/Array.h>

#include <utility>

///////////////////////////////////////////////////////////////////////////////

template <typename T>
class Resource
{
public:
	Resource() : mRID(0) { }
	virtual ~Resource() { }

	Resource(const Resource&) = delete;
	Resource& operator=(const Resource&) = delete;

	virtual Uint32 GetTypeID() const = 0;
	virtual const char* GetTypeName() const = 0;

	/* Get resource ID */
	Uint32 GetRID() const
	{
		return mRID;
	}

	/* Load resource from file */
	virtual bool LoadFromFile(const char* fname) { return true; }

public:
	/* Create resource pool */
	static void CreatePool(Uint32 size)
	{
		sObjectPool.Create(size, sizeof(T), alignof(T));
		sAllocated.Resize(size, true);
	}

	/* Free resource pool */
	static void FreePool()
	{
		// Free allocated resources
		T* start = (T*)sObjectPool.DataPtr();
		for (Uint32 i = 0; i < sAllocated.Size(); ++i)
		{
			if (sAllocated[i])
				start[i].~T();
		}

		sObjectPool.Free();
		sAllocated.Free();
	}


	/* Create resource */
	static T* Create()
	{
		// Allocate space
		T* object = (T*)sObjectPool.Alloc();
		if (!object) return 0;

		// Create resource
		new(object)T();
		sAllocated[sObjectPool.GetIndex(object)] = true;

		return object;
	}

	/* Load resource from file */
	static T* Load(const char* fname)
	{
		T* object = Create();
		if (!object) return 0;

		// Load resource
		if (!object->LoadFromFile(fname))
		{
			Free(object);
			return 0;
		}

		return object;
	}

	/* Free resource */
	static void Free(T* res)
	{
		sObjectPool.Free(res);
		sAllocated[sObjectPool.GetIndex(res)] = false;
	}

protected:
	/* Resource ID */
	Uint32 mRID;

protected:
	/* Resource pool */
	static ObjectPool sObjectPool;
	/* List marks which slots are used */
	static Array<bool> sAllocated;
};

///////////////////////////////////////////////////////////////////////////////

#include <Core/TypeInfo.h>

#define RESOURCE_TYPE(x) \
TYPE_INFO(x); friend Resource<x>;

#define INIT_RESOURCE_TYPE(x) \
ObjectPool Resource<x>::sObjectPool; \
Array<bool> Resource<x>::sAllocated;

///////////////////////////////////////////////////////////////////////////////

#endif