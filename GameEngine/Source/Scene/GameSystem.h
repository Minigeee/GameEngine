#ifndef GAME_SYSTEM_H
#define GAME_SYSTEM_H

#include <Core/TypeInfo.h>
#include <Core/LogFile.h>

#include <Scene/ComponentData.h>

#include <unordered_set>

///////////////////////////////////////////////////////////////////////////////

class Scene;

///////////////////////////////////////////////////////////////////////////////

/* Not really a list, just represents pointer to component data */
template <typename T>
struct ComponentList
{
	/* Convenience accessor operator */
	T& operator[](Uint32 i) { return mData[i]; }

	/* Pointer to data */
	T* mData;
	/* How many components there are */
	Uint32 mSize;
};

///////////////////////////////////////////////////////////////////////////////

class GameSystem
{
	REQUIRES_TYPE_INFO;

public:
	GameSystem();
	virtual ~GameSystem();

	/* Initialize system */
	void Init(Scene* scene);
	/* Clean up system */
	void CleanUp();

	/* Recieve updates every frame */
	virtual void Update(float dt);

	/* Override to add system dependencies */
	virtual void RegisterDependencies();

	/* Add an object type */
	void RegisterObjectType(Uint32 typeID, const std::unordered_set<Uint32>& set);

protected:
	/* Custom initialization */
	virtual void OnInit();
	/* Custom clean up */
	virtual void OnCleanUp();

	/* Get component lists from objects that meet requirements */
	template <typename T>
	Array<ComponentList<T>> GetComponentLists();

protected:
	/* Scene access */
	Scene* mScene;

private:
	/* Checks if the set of components matches system requirements */
	virtual bool MatchesRequirements(const std::unordered_set<Uint32>& set) = 0;

private:
	/* List of object types that meet system requirements */
	Array<Uint32> mObjectTypes;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline Array<ComponentList<T>> GameSystem::GetComponentLists()
{
	if (!mObjectTypes.Size())
	{
		LOG_ERROR << GetTypeName() << " failed to get component lists for type: " << T::StaticTypeName();
		return Array<ComponentList<T>>();
	}

	Array<ComponentList<T>> components(mObjectTypes.Size());

	for (Uint32 i = 0; i < mObjectTypes.Size(); ++i)
	{
		Array<T>& data = ComponentData<T>::GetData(mObjectTypes[i]);

		ComponentList<T> list;
		list.mData = &data.Front();
		list.mSize = data.Size();

		components.Push(list);
	}

	return components;
}

///////////////////////////////////////////////////////////////////////////////

#include <Core/Macros.h>


#define _MATCHES_REQUIREMENTS_FUNC(x) valid &= set.find(x::StaticTypeID()) != set.end();
#define _REQUIRES_COMPONENT_FUNC(x) template <> bool RequiresComponent<x>() const { return true; }


#define _REQUIRES_COMPONENTS_IMPL(...) \
public: \
	bool MatchesRequirements(const std::unordered_set<Uint32>& set) override \
	{ \
		bool valid = true; \
		LOOP(_MATCHES_REQUIREMENTS_FUNC, __VA_ARGS__) \
		return valid; \
	} \
	template <typename T> bool RequiresComponent() const { return false; } \
	LOOP(_REQUIRES_COMPONENT_FUNC, __VA_ARGS__)


#define REQUIRES_COMPONENTS(...) _REQUIRES_COMPONENTS_IMPL(__VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////

#endif