#ifndef COMPONENT_DATA_H
#define COMPONENT_DATA_H

#include <Core/Array.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////

struct GameObjectID;
struct Component;

///////////////////////////////////////////////////////////////////////////////

/* Object that is returned when game objects are created.
   They store pointers to the begginning of the create components */
class ComponentMap
{
public:
	/* Add component pointer to map */
	void Add(Uint32 type, Component* c) { mMap[type] = c; }
	/* Get component pointer */
	template <typename T> T* Get() { return (T*)mMap[T::StaticTypeID()]; }

private:
	std::unordered_map<Uint32, Component*> mMap;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T>
class ComponentData
{
public:
	/* Add component group and return index */
	static Uint32 CreateGroup()
	{
		if (!sData.Capacity())
			sData.Reserve(8);

		sData.Push(Array<T>(16));
		return sData.Size() - 1;
	}

	/* Create components for specific group (Don't call manually) */
	static T* CreateComponents(Uint32 group, const Array<GameObjectID>& ids)
	{
		assert(group < sData.Size());

		Array<T>& data = sData[group];

		for (Uint32 i = 0; i < ids.Size(); ++i)
			data.Push(T(ids[i]));

		return &data.Back() - ids.Size() + 1;
	}

	/* Remove components by index (Don't call manually) */
	static void RemoveComponents(Uint32 group, const Array<Uint32>& indices)
	{
		assert(group < sData.Size());

		Array<T>& data = sData[group];

		for (Uint32 i = 0; i < indices.Size(); ++i)
		{
			Uint32 id = indices[i];
			data.SwapPop(id);
		}
	}

	/* Get specific component (Don't call manually) */
	static T* GetComponent(Uint32 group, Uint32 index)
	{
		assert(group < sData.Size());
		return &sData[group][index];
	}

	/* Get component data */
	static const Array<Array<T>>& GetData()
	{
		return sData;
	}

	/* Reset data */
	static void Reset()
	{
		sData.Clear();
	}

private:
	/* Component data */
	static Array<Array<T>> sData;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T>
Array<Array<T>> ComponentData<T>::sData;

///////////////////////////////////////////////////////////////////////////////

#endif