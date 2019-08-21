#include <Game/WorldScene.h>

#include <Core/LogFile.h>

#include <Game/Systems/InputSystem.h>

#include <Game/Objects/PlayerObject.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

WorldScene::WorldScene()
{

}

WorldScene::~WorldScene()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include <Graphics/Terrain.h>

void WorldScene::OnCreate()
{
	LOG << "Creating world\n";

	Array<PlayerObject*> objects = CreateObjects<PlayerObject>(1);
	mRenderer.AddDynamic(objects[0]);

	InputSystem* system = RegisterSystem<InputSystem>();
	system->SetMainPlayer(objects[0]);

	mDirLight.SetDirection(0.0f, -1.0f, 0.2f);
	mCamera.SetPosition(0.0f, 2.0f, 4.0f);


	Array<float> lod(8);
	lod.Push(20.0f);
	lod.Push(50.0f);
	lod.Push(100.0f);
	lod.Push(200.0f);

	Terrain* terrain = Resource<Terrain>::Create();
	terrain->SetLodLevels(lod);
	terrain->SetSquareSize(2.0f);
	terrain->Create();
	mRenderer.AddStatic(terrain);


	// PP effects
	mPostProcess.Enable();
}

///////////////////////////////////////////////////////////////////////////////

void WorldScene::OnDelete()
{
	LOG << "Deleting world\n";
}

///////////////////////////////////////////////////////////////////////////////