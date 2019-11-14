#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <Core/DataTypes.h>
#include <Core/TypeInfo.h>
#include <Core/HandleArray.h>

#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////

class Scene;

///////////////////////////////////////////////////////////////////////////////

struct GameObjectID
{
public:
	GameObjectID() = default;
	GameObjectID(Handle handle, Uint16 typeID);

	operator Uint32() const;

	/* Get object handle */
	Handle Handle() const;
	/* Get object type ID */
	Uint16 TypeID() const;

private:
	/* Object handle */
	::Handle mHandle;
	/* Object type ID */
	Uint16 mTypeID;
};

///////////////////////////////////////////////////////////////////////////////

/* Convenient component accessor class. Don't use excessively. */
class GameObject
{
	REQUIRES_TYPE_INFO;

public:
	GameObject() = default;
	GameObject(Scene* scene, GameObjectID id);

	/* Get a component */
	template <typename T> T* GetComponent() const;

	/* Get object ID */
	GameObjectID GetID() const;

private:
	/* Scene access */
	Scene* mScene;	
	/* Object ID */
	GameObjectID mID;
};

///////////////////////////////////////////////////////////////////////////////

#include <Scene/Scene.h>

template <typename T>
inline T* GameObject::GetComponent() const { return mScene->GetComponent<T>(mID); }

///////////////////////////////////////////////////////////////////////////////

#include <Core/Macros.h>

#include <Scene/ComponentData.h>

struct Component;

///////////////////////////////////////////////////////////////////////////////

#define _CREATE_COMPONENT_GROUPS_FUNC(x) \
	groups.Push(ComponentData<x>::CreateGroup()); \
	typeToGroup[x::StaticTypeID()] = groups.Back();
#define _CREATE_COMPONENTS_FUNC(x) map.Add(x::StaticTypeID(), (Component*)ComponentData<x>::CreateComponents(*++group, ids));
#define _REMOVE_COMPONENTS_FUNC(x) ComponentData<x>::RemoveComponents(*++group, indices);


#define _REGISTER_COMPONENTS_IMPL(...) \
public: \
	static void CreateComponentGroups(Array<Uint32>& groups, std::unordered_map<Uint32, Uint32>& typeToGroup) \
	{ groups.Reserve(NARGS(__VA_ARGS__)); LOOP(_CREATE_COMPONENT_GROUPS_FUNC, __VA_ARGS__) } \
	static ComponentMap CreateComponents(const Array<Uint32>& groups, const Array<GameObjectID>& ids) \
	{ \
		ComponentMap map; \
		Uint32* group = &groups.Front() - 1; \
		LOOP(_CREATE_COMPONENTS_FUNC, __VA_ARGS__) \
		return map; \
	} \
	static void RemoveComponents(const Array<Uint32>& groups, const Array<Uint32>& indices) \
	{ Uint32* group = &groups.Front() - 1; LOOP(_REMOVE_COMPONENTS_FUNC, __VA_ARGS__) } \

#define REGISTER_COMPONENTS(...) _REGISTER_COMPONENTS_IMPL(__VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////

#define GAME_OBJECT(x) TYPE_INFO(x); using GameObject::GameObject;

///////////////////////////////////////////////////////////////////////////////

#endif