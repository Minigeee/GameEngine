#include <Scene/Scene.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Scene::Scene() :
	mEngine		(0)
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
	mRenderer.SetCamera(&mCamera);

	OnCreate();
}

///////////////////////////////////////////////////////////////////////////////

void Scene::Delete()
{
	OnDelete();
}

///////////////////////////////////////////////////////////////////////////////

void Scene::Update(float dt)
{
	mRenderer.Render();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Scene::OnCreate()
{

}

void Scene::OnDelete()
{

}

///////////////////////////////////////////////////////////////////////////////