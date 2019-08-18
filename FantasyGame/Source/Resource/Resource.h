#ifndef RESOURCE_H
#define RESOURCE_H

#include <Core/ObjectPool.h>
#include <Core/StringHash.h>

#include <Resource/Loadable.h>

#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////

template< typename C, typename = void >
struct has_receive
	: std::false_type
{};

template< typename C >
struct has_receive< C, typename std::enable_if<
	std::is_same<
	decltype(std::declval<C>().GetFileHash()),
	StringHash
	>::value
>::type >
	: std::true_type
{};

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
		// Check if file has already been loaded
		Uint32 hash((Uint32)StringHash(fname));
		auto it = sFileMap.find(hash);
		if (it != sFileMap.end())
			// Return loaded resource
			return it->second;

		T* obj = sResourcePool.New();
		if (!obj->Load(fname, args...))
		{
			sResourcePool.Free(obj);
			return 0;
		}

		// Added to loaded files
		sFileMap[hash] = obj;

		return obj;
	}

	static void Free(T* resource)
	{
		if (dynamic_cast<Loadable*>(resource))
			FreeLoadable((Loadable*)resource);

		sResourcePool.Free(resource);
	}

private:
	static void FreeLoadable(Loadable* resource)
	{
		// Remove from loaded files if needed
		Uint32 hash(resource->GetFileHash());
		auto it = sFileMap.find(hash);
		if (it != sFileMap.end())
			sFileMap.erase(it);
	}

private:
	/* Resource pool */
	static ObjectPool<T> sResourcePool;
	/* Loaded resources */
	static std::unordered_map<Uint32, T*> sFileMap;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T>
ObjectPool<T> Resource<T>::sResourcePool;

template <typename T>
std::unordered_map<Uint32, T*> Resource<T>::sFileMap;

///////////////////////////////////////////////////////////////////////////////

#endif