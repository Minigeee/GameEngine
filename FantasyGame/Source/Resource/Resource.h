#ifndef RESOURCE_H
#define RESOURCE_H

#include <Core/ObjectPool.h>

///////////////////////////////////////////////////////////////////////////////

template <typename T>
class Resource
{
public:
	static T* Create()
	{
		T* obj = sResourcePool.New();

		return obj;
	}

	template <typename... Args>
	static T* Load(const char* fname, Args... args)
	{
		T* obj = sResourcePool.New();
		if (!obj->Load(fname, args...))
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