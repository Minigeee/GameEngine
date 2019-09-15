#include <Game/WorldScene.h>

#include <Core/LogFile.h>

#include <Graphics/Atmosphere.h>

#include <Game/Systems/InputSystem.h>
#include <Game/Systems/TerrainSystem.h>

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

	// Systems
	InputSystem* system = RegisterSystem<InputSystem>();
	system->SetMainPlayer(objects[0]);

	RegisterSystem<TerrainSystem>();


	mDirLight.SetDirection(0.0f, -0.5f, 1.0f);
	mCamera.SetPosition(0.0f, 2.0f, 4.0f);


	// PP effects
	mPostProcess.Enable();

	Atmosphere* atm = mPostProcess.AddEffect<Atmosphere>();
	atm->Init(this);
}

///////////////////////////////////////////////////////////////////////////////

void WorldScene::OnDelete()
{
	LOG << "Deleting world\n";
}

///////////////////////////////////////////////////////////////////////////////