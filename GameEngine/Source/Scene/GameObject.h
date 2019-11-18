#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <Core/DataTypes.h>
#include <Core/TypeInfo.h>
#include <Core/HandleArray.h>
#include <Core/StringHash.h>

#include <unordered_map>
#include <unordered_set>

///////////////////////////////////////////////////////////////////////////////

class Scene;

///////////////////////////////////////////////////////////////////////////////

struct GameObjectID
{
public:
	GameObjectID();
	GameObjectID(Handle handle, Uint16 typeID);

	operator Uint32() const;

	/* Get object handle */
	Handle Handle() const;
	/* Get object type ID */
	Uint16 TypeID() const;
	/* Returns if object has been created */
	bool Exists() const;

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

	/* Get object ID */
	GameObjectID GetID() const;

private:
	/* Scene access */
	Scene* mScene;	
	/* Object ID */
	GameObjectID mID;
};

///////////////////////////////////////////////////////////////////////////////

#include <Core/Macros.h>

#include <Scene/ComponentData.h>

struct Component;

///////////////////////////////////////////////////////////////////////////////

#define _CREATE_COMPONENT_GROUPS_FUNC(x) ComponentData<x>::CreateGroup(typeID);
#define _CREATE_COMPONENTS_FUNC(x) map.Add(x::StaticTypeID(), (Component*)ComponentData<x>::CreateComponents(typeID, ids));
#define _REMOVE_COMPONENTS_FUNC(x) ComponentData<x>::RemoveComponents(typeID, indices);
#define _GET_COMPONENT_TYPES_FUNC(x) set.insert(x::StaticTypeID());
#define _HAS_COMPONENT_FUNC(x) template <> static bool HasComponent<x>() { return true; }
#define _ADD_OBJECT_TAGS(x) sTags.insert((Uint32)StringHash(x));


#define _REGISTER_COMPONENTS_IMPL(...) \
public: \
	static void CreateComponentGroups() \
	{ Uint32 typeID = StaticTypeID(); LOOP(_CREATE_COMPONENT_GROUPS_FUNC, __VA_ARGS__) } \
	static ComponentMap CreateComponents(const Array<GameObjectID>& ids) \
	{ \
		ComponentMap map; Uint32 typeID = StaticTypeID(); \
		LOOP(_CREATE_COMPONENTS_FUNC, __VA_ARGS__) \
		return map; \
	} \
	static void RemoveComponents(const Array<Uint32>& indices) \
	{ Uint32 typeID = StaticTypeID(); LOOP(_REMOVE_COMPONENTS_FUNC, __VA_ARGS__) } \
	static void GetComponentTypes(std::unordered_set<Uint32>& set) \
	{ LOOP(_GET_COMPONENT_TYPES_FUNC, __VA_ARGS__) } \
	template <typename T> static bool HasComponent() { return false; } \
	LOOP(_HAS_COMPONENT_FUNC, __VA_ARGS__)

#define REGISTER_COMPONENTS(...) _REGISTER_COMPONENTS_IMPL(__VA_ARGS__)


#define _REGISTER_TAGS_IMPL(...) \
public: \
	static std::unordered_set<Uint32>& GetTags() \
	{ if (sTags.empty()) { LOOP(_ADD_OBJECT_TAGS, __VA_ARGS__) } return sTags; } \
	static bool HasTag(StringHash tag) { return sTags.find((Uint32)tag) != sTags.end(); } \
private: \
	static std::unordered_set<Uint32> sTags;

#define REGISTER_TAGS(...) _REGISTER_TAGS_IMPL(__VA_ARGS__)


#define INIT_GAME_OBJECT(x) \
std::unordered_set<Uint32> x::sTags;

///////////////////////////////////////////////////////////////////////////////

#define GAME_OBJECT(x) TYPE_INFO(x); using GameObject::GameObject;

///////////////////////////////////////////////////////////////////////////////

#endif