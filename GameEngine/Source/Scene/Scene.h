#ifndef SCENE_H
#define SCENE_H

#include <Core/HandleArray.h>

#include <Engine/Input.h>

#include <Graphics/Renderer.h>
#include <Graphics/Camera.h>
#include <Graphics/Lights.h>
#include <Graphics/PostProcess.h>

#include <Resource/Resource.h>

#include <unordered_map>
#include <unordered_set>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////

class Engine;
class EventListener;
class GameSystem;
class GameObject;
class ObjectLoader;
class Skybox;

struct GameObjectID;
class ComponentMap;

///////////////////////////////////////////////////////////////////////////////

struct ObjectData
{
	/* Keeps track of game object handles */
	HandleArray<bool> mObjectHandles;
	/* The set of component types the game object contains */
	std::unordered_set<Uint32> mComponentTypes;
	/* Remove function */
	void (*mRemoveFunc)(const Array<Uint32>&);
};

///////////////////////////////////////////////////////////////////////////////

class Scene
{
	friend Input;

public:
	Scene();
	virtual ~Scene();

	/* Create scene */
	void Create(Engine* engine);
	/* Delete scene */
	void Delete();
	/* Update scene and do game logic */
	void Update(float dt);

	/* Get engine pointer */
	Engine* GetEngine() const;
	/* Get rendering system */
	Renderer& GetRenderer();
	/* Get post processing system */
	PostProcess& GetPostProcess();

	/* Get scene camera */
	Camera& GetCamera();
	/* Get ambient color */
	Vector3f& GetAmbient();
	/* Get directional light */
	DirLight& GetDirLight();
	/* Get skybox */
	Skybox* GetSkybox() const;

	/* Add renderable object */
	void AddRenderable(Renderable* object, bool isStatic);


	/* ====================== Event System ====================== */

	/* Send event */
	void SendEvent(const void* event, Uint32 type);
	/* Send event */
	template <typename T> void SendEvent(const T& event) { SendEvent(&event, T::StaticTypeID()); }
	/* Register listener to certain event list */
	void RegisterListener(EventListener* listener, Uint32 type);
	/* Register listener to certain event list */
	template <typename T> void RegisterListener(EventListener* listener) { RegisterListener(listener, T::StaticTypeID()); }
	/* Register listener to all registered events */
	void RegisterListener(EventListener* listener);

	/* ====================== Game Systems ====================== */

	/* Register game system */
	template <typename T> T* RegisterSystem();
	/* Register game system */
	bool RegisterSystem(GameSystem* system, Uint32 type);

	/* Access system */
	template <typename T> T* GetSystem() const { return (T*)GetSystem(T::StaticTypeID()); }
	/* Access system */
	GameSystem* GetSystem(Uint32 type) const;


	/* ====================== Game Objects ====================== */

	/* Register object type */
	template <typename T> void RegisterObject();
	/* Create game objects (Provide components map to edit components upon creation) */
	template <typename T>
	Array<GameObjectID> CreateObjects(Uint32 num, ComponentMap* components = 0);
	/* Get game object from ID */
	template <typename T> T GetObject(GameObjectID id);
	/* Access component using game object ID */
	template <typename T> T* GetComponent(GameObjectID id);
	/* Remove a list of game objects */
	template <typename T> void RemoveObjects(const Array<GameObjectID>& ids);
	/* Queue removal of an object */
	void QueueRemoveObject(GameObjectID id);

	/* ====================== Object Loaders ====================== */

	/* Register object loader */
	template <typename T> T* RegisterLoader()
	{
		T* loader = new T();
		if (!RegisterLoader(loader, T::StaticTypeID()))
		{
			delete loader;
			return 0;
		}

		return loader;
	}
	/* Register object loader */
	bool RegisterLoader(ObjectLoader* loader, Uint32 type);

	/* Access object loader */
	template <typename T> T* GetLoader() const { return (T*)GetLoader(T::StaticTypeID()); }
	/* Access object loader */
	ObjectLoader* GetLoader(Uint32 type) const;


protected:
	/* Handle key events */
	virtual void OnKeyEvent(const E_KeyEvent& event);
	/* Handle mouse move events */
	virtual void OnMouseMove(const E_MouseMove& event);
	/* Handle mouse button events */
	virtual void OnMouseButton(const E_MouseButton& event);
	/* Handle mouse scroll events */
	virtual void OnMouseScroll(const E_MouseScroll& event);

protected:
	/* Access to engine */
	Engine* mEngine;
	/* Scene renderer */
	Renderer mRenderer;
	/* Post processing system */
	PostProcess mPostProcess;

	/* Scene camera */
	Camera mCamera;
	/* Ambient color */
	Vector3f mAmbientColor;
	/* Directional light */
	DirLight mDirLight;
	/* Skybox */
	Skybox* mSkybox;

private:
	/* Called on scene creation */
	virtual void OnCreate();
	/* Called when scene is deleted */
	virtual void OnDelete();

	/* Remove objects from removal queue */
	void RemoveQueuedObjects();

private:
	/* Map of event listeners */
	std::unordered_map<Uint32, Array<EventListener*>> mListeners;
	/* Map of game systems */
	std::unordered_map<Uint32, GameSystem*> mSystems;
	/* Map of object loaders */
	std::unordered_map<Uint32, ObjectLoader*> mLoaders;

	/* Update list for game systems */
	Array<GameSystem*> mSystemUpdateList;
	/* Update list for object loaders */
	Array<ObjectLoader*> mLoaderUpdateList;

	/* Maps object type to data */
	std::unordered_map<Uint32, ObjectData> mTypeToObjectData;
	/* List of game objects to remove */
	Array<GameObjectID> mRemovalQueue;
};

///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include <Scene/GameObject.h>
#include <Scene/ComponentData.h>

///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline T* Scene::RegisterSystem()
{
	T* ptr = new T();
	if (!RegisterSystem(ptr, T::StaticTypeID()))
	{
		delete ptr;
		return 0;
	}

	return ptr;
}

///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void Scene::RegisterObject()
{
	Uint32 typeID = T::StaticTypeID();
	ObjectData& data = mTypeToObjectData[typeID];

	if (!data.mObjectHandles.Capacity())
	{
		// Initialize data
		data.mObjectHandles.Reserve(32);
		data.mRemoveFunc = T::RemoveComponents;

		// Create component groups
		T::CreateComponentGroups();
		// Get component type IDs
		T::GetComponentTypes(data.mComponentTypes);

		// Add this object type to any systems that will use it
		for (auto it = mSystems.begin(); it != mSystems.end(); ++it)
			it->second->RegisterObjectType(
				T::StaticTypeID(),
				data.mComponentTypes,
				T::GetTags());
	}
}


template <typename T>
inline Array<GameObjectID> Scene::CreateObjects(Uint32 num, ComponentMap* components)
{
	Uint32 typeID = T::StaticTypeID();
	ObjectData& data = mTypeToObjectData[typeID];

	if (!data.mObjectHandles.Capacity())
	{
		// Register game object if not registered
		RegisterObject<T>();
		data = mTypeToObjectData[typeID];
	}

	Array<GameObjectID> ids(num);
	// Create game object IDs
	for (Uint32 i = 0; i < num; ++i)
		ids.Push(GameObjectID(data.mObjectHandles.Add(true), (Uint16)typeID));

	// Create components
	if (components)
		*components = T::CreateComponents(ids);
	else
		T::CreateComponents(ids);

	return ids;
}


template <typename T>
T Scene::GetObject(GameObjectID id)
{
	assert(id.TypeID() == T::StaticTypeID());
	T obj(this, id);
	return obj;
}


template <typename T>
T* Scene::GetComponent(GameObjectID id)
{
	ObjectData& data = mTypeToObjectData[id.TypeID()];

	return ComponentData<T>::GetComponent(
		(Uint32)id.TypeID(),
		data.mObjectHandles.HandleToIndex(id.Handle())
	);
}


template <typename T>
inline void Scene::RemoveObjects(const Array<GameObjectID>& ids)
{
	Uint32 typeID = T::StaticTypeID();
	ObjectData& data = mTypeToObjectData[typeID];

	// Keep track of which indices were removed
	Array<Uint32> indices(ids.Size());

	for (Uint32 i = 0; i < ids.Size(); ++i)
	{
		Handle handle = ids[i].Handle();
		indices.Push(data.mObjectHandles.HandleToIndex(handle));
		data.mObjectHandles.Remove(handle);
	}

	// Remove components
	T::RemoveComponents(indices);
}

///////////////////////////////////////////////////////////////////////////////

#endif