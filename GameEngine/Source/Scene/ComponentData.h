#ifndef COMPONENT_DATA_H
#define COMPONENT_DATA_H

#include <Core/Array.h>

#include <Scene/GameObject.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////

template <typename T>
class ComponentData
{
public:
	/* Add component group and return index */
	static Uint32 CreateGroup()
	{
		sData.Push(Array<T>(16));
		return sData.Size() - 1;
	}

	/* Create components for specific group (Don't call manually) */
	static T* CreateComponents(Uint32 group, const Array<GameObjectID>& ids)
	{
		assert(group < sData.Size());

		Array<T>& data = sData[group];
		T* start = &data.Back();

		for (Uint32 i = 0; i < ids.Size(); ++i)
			data.Push(T(ids[i]));

		return start + 1;
	}

	/* Remove components by index (Don't call manually) */
	static void RemoveComponents(Uint32 group, const Array<Uint32>& indices)
	{
		assert(group < sData.Size());

		Array<T>& data = sData[group];

		for (Uint32 i = 0; i < indices.Size(); ++i)
			data.SwapPop(indices[i]);
	}

	/* Get specific component (Don't call manually) */
	static T* GetComponent(Uint32 group, Uint32 index)
	{
		assert(group < sData.Size());
		return sData[group][index];
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

#endif