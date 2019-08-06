#ifndef SCENE_H
#define SCENE_H

#include <Graphics/Renderer.h>
#include <Graphics/Camera.h>
#include <Graphics/Lights.h>

#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////

class Engine;
class EventListener;

class Scene
{
public:
	Scene();
	virtual ~Scene();

	/* Create scene */
	void Create(Engine* engine);
	/* Delete scene */
	void Delete();
	/* Update scene and do game logic */
	void Update(float dt);

	/* Get scene camera */
	Camera& GetCamera();
	/* Get ambient color */
	Vector3f& GetAmbient();
	/* Get directional light */
	DirLight& GetDirLight();

	/* Send event */
	void SendEvent(const void* event, Uint32 type);
	/* Send event */
	template <typename T> void SendEvent(const T& event) { SendEvent(&event, T::StaticTypeID()); }
	/* Add listener to certain event list */
	void AddListener(EventListener* listener, Uint32 type);
	/* Add listener to certain event list */
	template <typename T> void AddListener(EventListener* listener) { AddListener(listener, T::StaticTypeID()); }
	/* Add listener to all registered events */
	void AddListener(EventListener* listener);

protected:
	/* Access to engine */
	Engine* mEngine;
	/* Scene renderer */
	Renderer mRenderer;

	/* Scene camera */
	Camera mCamera;
	/* Ambient color */
	Vector3f mAmbientColor;
	/* Directional light */
	DirLight mDirLight;

private:
	/* Called on scene creation */
	virtual void OnCreate();
	/* Called when scene is deleted */
	virtual void OnDelete();

private:
	/* Map of event listeners */
	std::unordered_map<Uint32, Array<EventListener*>> mListeners;
};

///////////////////////////////////////////////////////////////////////////////

#endif