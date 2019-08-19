#ifndef SCENE_H
#define SCENE_H

#include <Engine/Input.h>

#include <Graphics/Renderer.h>
#include <Graphics/Camera.h>
#include <Graphics/Lights.h>
#include <Graphics/PostProcess.h>

#include <Resource/Resource.h>

#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////

class Engine;
class EventListener;
class GameSystem;
class GameObject;

class Skybox;

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
	/* Get scene camera */
	Camera& GetCamera();
	/* Get ambient color */
	Vector3f& GetAmbient();
	/* Get directional light */
	DirLight& GetDirLight();
	/* Get skybox */
	Skybox* GetSkybox() const;


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
	template <typename T> T* RegisterSystem()
	{
		T* ptr = new T();
		if (!RegisterSystem(ptr, T::StaticTypeID()))
		{
			delete ptr;
			return 0;
		}

		return ptr;
	}
	/* Register game system */
	bool RegisterSystem(GameSystem* system, Uint32 type);

	/* Access system */
	template <typename T> T* GetSystem() const { return (T*)GetSystem(T::StaticTypeID()); }
	/* Access system */
	GameSystem* GetSystem(Uint32 type) const;


	/* ====================== Game Objects ====================== */

	/* Create game objects */
	template <typename T>
	Array<T*> CreateObjects(Uint32 num)
	{
		Array<T*> objects(num);

		for (Uint32 i = 0; i < num; ++i)
		{
			T* object = Resource<T>::Create();
			object->Create();
			objects.Push(object);
		}

		return objects;
	}

	/* Free game object */
	template <typename T>
	void FreeObject(T* object)
	{
		Resource<T>::Free(object);
	}


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

private:
	/* Map of event listeners */
	std::unordered_map<Uint32, Array<EventListener*>> mListeners;
	/* Map of game systems */
	std::unordered_map<Uint32, GameSystem*> mSystems;
	/* Update list for game systems */
	Array<GameSystem*> mUpdateList;
};

///////////////////////////////////////////////////////////////////////////////

#endif