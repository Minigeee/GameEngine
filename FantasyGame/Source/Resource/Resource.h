#ifndef RESOURCE_H
#define RESOURCE_H

#include <Core/ObjectPool.h>

///////////////////////////////////////////////////////////////////////////////

template <typename T>
class Resource
{
public:
	static T* Load(const char* fname)
	{
		T* obj = sResourcePool.New();
		if (!obj->Load(fname))
		{
			sResourcePool.Free(obj);
			return 0;
		}

		return obj;
	}

	static void Free(T* resource)
	{
		sResourcePool.Free(resource);
	}

private:
	/* Resource pool */
	static ObjectPool<T> sResourcePool;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T>
ObjectPool<T> Resource<T>::sResourcePool;

///////////////////////////////////////////////////////////////////////////////

#endif