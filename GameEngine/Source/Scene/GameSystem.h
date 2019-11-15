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
	virtual void Update(float dt) = 0;

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
#define _DEFINE_COMPONENT_LISTS_FUNC(x) Array<ComponentList<x>> CONCAT(_, x) = GetComponentLists<x>();
#define _GET_COMPONENT_LIST_REF_FUNC(x) ComponentList<x>& CONCAT(ref_, x) = CONCAT(_, x)[i];
#define _EXECUTE_SYSTEM_FUNC(x) CONCAT(ref_, x)[n]
#define _EXECUTE_SYSTEM_COMMA_FUNC(x) , CONCAT(ref_, x)[n]


#define _SYSTEM_UPDATE_IMPL(...) \
	void Update(float dt) override \
	{ \
		LOOP(_DEFINE_COMPONENT_LISTS_FUNC, __VA_ARGS__) \
		for (Uint32 i = 0; i < CONCAT(_, FIRST_ARG(__VA_ARGS__)).Size(); ++i) \
		{ \
			LOOP(_GET_COMPONENT_LIST_REF_FUNC, __VA_ARGS__) \
			for (Uint32 n = 0; n < CONCAT(ref_, FIRST_ARG(__VA_ARGS__)).mSize; ++n) \
			{ Execute(COMMA_LIST(_EXECUTE_SYSTEM_FUNC, _EXECUTE_SYSTEM_COMMA_FUNC, __VA_ARGS__)); } \
		} \
	} \

#define _REQUIRES_COMPONENTS_NO_UPDATE_IMPL(...) \
public: \
	bool MatchesRequirements(const std::unordered_set<Uint32>& set) override \
	{ \
		bool valid = true; \
		LOOP(_MATCHES_REQUIREMENTS_FUNC, __VA_ARGS__) \
		return valid; \
	} \
	template <typename T> bool RequiresComponent() const { return false; } \
	LOOP(_REQUIRES_COMPONENT_FUNC, __VA_ARGS__)

#define _REQUIRES_COMPONENTS_IMPL(...) \
	_REQUIRES_COMPONENTS_NO_UPDATE_IMPL(__VA_ARGS__) \
	_SYSTEM_UPDATE_IMPL(__VA_ARGS__)


#define REQUIRES_COMPONENTS(...) _REQUIRES_COMPONENTS_IMPL(__VA_ARGS__)
#define REQUIRES_COMPONENTS_CUSTOM_UPDATE(...) _REQUIRES_COMPONENTS_NO_UPDATE_IMPL(__VA_ARGS__)
#define REQUIRES_NO_COMPONENTS \
	bool MatchesRequirements(const std::unordered_set<Uint32>& set) override { return false; } \
	template <typename T> bool RequiresComponent() const { return false; } \

///////////////////////////////////////////////////////////////////////////////

#endif