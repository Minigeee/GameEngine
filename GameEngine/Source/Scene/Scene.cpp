#include <Scene/Scene.h>

#include <Engine/Engine.h>

#include <Scene/EventListener.h>
#include <Scene/GameSystem.h>
#include <Scene/ObjectLoader.h>

#include <Graphics/Skybox.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Scene::Scene() :
	mEngine			(0),
	mAmbientColor	(0.05f)
{

}

Scene::~Scene()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Scene::Create(Engine* engine)
{
	mEngine = engine;

	mRenderer.Init(this);

	mSystemUpdateList.Reserve(16);
	mLoaderUpdateList.Reserve(16);
	mRemovalQueue.Reserve(128);

	// Create skybox
	mSkybox = new Skybox();

	OnCreate();

	mRenderer.PostInit();
}

///////////////////////////////////////////////////////////////////////////////

void Scene::Delete()
{
	OnDelete();

	for (auto it = mSystems.begin(); it != mSystems.end(); ++it)
		delete it->second;

	for (auto it = mLoaders.begin(); it != mLoaders.end(); ++it)
		delete it->second;

	delete mSkybox;
}

///////////////////////////////////////////////////////////////////////////////

void Scene::Update(float dt)
{
	START_PROFILER(LoaderUpdate);
	for (Uint32 i = 0; i < mLoaderUpdateList.Size(); ++i)
		mLoaderUpdateList[i]->Update();
	STOP_PROFILER(LoaderUpdate);

	START_PROFILER(SystemUpdate);
	for (Uint32 i = 0; i < mSystemUpdateList.Size(); ++i)
		mSystemUpdateList[i]->Update(dt);
	STOP_PROFILER(SystemUpdate);

	// Remove objects in the removal queue
	RemoveQueuedObjects();
}

///////////////////////////////////////////////////////////////////////////////

void Scene::Render()
{
	// Render scene
	mRenderer.Render(mPostProcess.GetInput());
	// Render post process effects
	mPostProcess.Render();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Engine* Scene::GetEngine() const
{
	return mEngine;
}

Renderer& Scene::GetRenderer()
{
	return mRenderer;
}

Camera& Scene::GetCamera()
{
	return mCamera;
}

Vector3f& Scene::GetAmbient()
{
	return mAmbientColor;
}

DirLight& Scene::GetDirLight()
{
	return mDirLight;
}

Skybox* Scene::GetSkybox() const
{
	return mSkybox;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Scene::RegisterListener(EventListener* listener, Uint32 type)
{
	Array<EventListener*>& list = mListeners[type];
	if (!list.Capacity())
		list.Reserve(16);

	list.Push(listener);
}

void Scene::RegisterListener(EventListener* listener)
{
	listener->RegisterEvents(this);
}

///////////////////////////////////////////////////////////////////////////////

void Scene::SendEvent(const void* event, Uint32 type)
{
	Array<EventListener*>& list = mListeners[type];

	for (Uint32 i = 0; i < list.Size(); ++i)
		list[i]->HandleEvent(event, type);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Scene::QueueRemoveObject(GameObjectID id)
{
	mRemovalQueue.Push(id);
}

///////////////////////////////////////////////////////////////////////////////

void Scene::RemoveQueuedObjects()
{
	if (!mRemovalQueue.Size()) return;

	std::unordered_map<Uint32, Array<Uint32>> indicesMap;

	for (Uint32 i = 0; i < mRemovalQueue.Size(); ++i)
	{
		GameObjectID id = mRemovalQueue[i];
		Uint32 typeID = id.TypeID();
		Handle handle = id.Handle();

		ObjectData& data = mTypeToObjectData[typeID];
		Array<Uint32>& indices = indicesMap[typeID];

		if (!indices.Capacity())
			indices.Reserve(32);

		indices.Push(data.mObjectHandles.HandleToIndex(handle));
		data.mObjectHandles.Remove(handle);
	}

	// Remove components
	for (auto it = indicesMap.begin(); it != indicesMap.end(); ++it)
		(*mTypeToObjectData[it->first].mRemoveFunc)(it->second);

	// Reset queue
	mRemovalQueue.Clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool Scene::RegisterSystem(GameSystem* system, Uint32 type)
{
	GameSystem*& ptr = mSystems[type];
	if (ptr) return false;

	ptr = system;
	system->Init(this);
	system->RegisterDependencies();

	// Add to update list
	mSystemUpdateList.Push(system);

	return true;
}

///////////////////////////////////////////////////////////////////////////////

GameSystem* Scene::GetSystem(Uint32 type) const
{
	auto it = mSystems.find(type);
	return it == mSystems.end() ? 0 : it->second;
}

///////////////////////////////////////////////////////////////////////////////

bool Scene::RegisterLoader(ObjectLoader* loader, Uint32 type)
{
	ObjectLoader*& ptr = mLoaders[type];
	// Loader has already been added for this type
	if (ptr) return false;

	ptr = loader;
	loader->Init(this);

	// Add to update list
	mLoaderUpdateList.Push(loader);

	return true;
}

///////////////////////////////////////////////////////////////////////////////

ObjectLoader* Scene::GetLoader(Uint32 type) const
{
	auto it = mLoaders.find(type);
	return it == mLoaders.end() ? 0 : it->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Scene::OnKeyEvent(const E_KeyEvent& event)
{
	/* TEMP */

	if (event.mAction == Input::Press && event.mKey == Input::Escape)
		mEngine->Close();

	/* \TEMP */

	SendEvent(event);
}

void Scene::OnMouseMove(const E_MouseMove& event)
{
	SendEvent(event);
}

void Scene::OnMouseButton(const E_MouseButton& event)
{
	SendEvent(event);
}

void Scene::OnMouseScroll(const E_MouseScroll& event)
{
	SendEvent(event);
}

///////////////////////////////////////////////////////////////////////////////

void Scene::OnCreate()
{

}

void Scene::OnDelete()
{

}

///////////////////////////////////////////////////////////////////////////////