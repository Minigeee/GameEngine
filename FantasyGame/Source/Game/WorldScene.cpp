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

#include <Graphics/Image.h>

void WorldScene::OnCreate()
{
	LOG << "Creating world\n";

	Array<PlayerObject*> objects = CreateObjects<PlayerObject>(1);
	mRenderer.AddDynamic(objects[0]);

	InputSystem* system = RegisterSystem<InputSystem>();
	system->SetMainPlayer(objects[0]);

	Image* image = Resource<Image>::Load("Textures/YourName.jpg");


	mDirLight.SetDirection(0.0f, -1.0f, 2.0f);
	mCamera.SetPosition(0.0f, 2.0f, 4.0f);
}

///////////////////////////////////////////////////////////////////////////////

void WorldScene::OnDelete()
{
	LOG << "Deleting world\n";
}

///////////////////////////////////////////////////////////////////////////////