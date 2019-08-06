#include <Scene/Scene.h>

#include <Engine/Engine.h>

#include <Scene/EventListener.h>
#include <Scene/GameSystem.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Scene::Scene() :
	mEngine			(0),
	mAmbientColor	(0.1f)
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

	mRenderer.Init();
	mRenderer.SetScene(this);

	mUpdateList.Resize(16);

	OnCreate();
}

///////////////////////////////////////////////////////////////////////////////

void Scene::Delete()
{
	for (auto it = mSystems.begin(); it != mSystems.end(); ++it)
		delete it->second;

	OnDelete();
}

///////////////////////////////////////////////////////////////////////////////

void Scene::Update(float dt)
{
	for (Uint32 i = 0; i < mUpdateList.Size(); ++i)
		mUpdateList[i]->Update(dt);

	/* TEMP */

	static float time = 0.0f;
	time += dt;

	float x = sin(time * 0.5f) * 6.0f;
	float z = cos(time * 0.5f) * 10.0f;
	mCamera.SetPosition(x, 4.0f, 10.0f);

	mRenderer.Render();

	/* /TEMP */
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Scene::RegisterListener(EventListener* listener, Uint32 type)
{
	Array<EventListener*>& list = mListeners[type];
	if (!list.Capacity())
		list.Resize(16);

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

bool Scene::RegisterSystem(GameSystem* system, Uint32 type)
{
	GameSystem*& ptr = mSystems[type];
	if (ptr) return false;

	ptr = system;
	system->Init(this);
	system->RegisterDependencies();

	// Add to update list
	mUpdateList.Push(system);

	return true;
}

///////////////////////////////////////////////////////////////////////////////

GameSystem* Scene::GetSystem(Uint32 type) const
{
	auto it = mSystems.find(type);
	return it == mSystems.end() ? 0 : it->second;
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