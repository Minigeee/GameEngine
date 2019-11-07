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
	mRenderer.AddStaticObject(objects[0]);

	// Systems
	InputSystem* system = RegisterSystem<InputSystem>();
	system->SetMainPlayer(objects[0]);

	RegisterSystem<TerrainSystem>();


	mDirLight.SetDirection(0.0f, -0.1f, 1.0f);
	mCamera.SetPosition(0.0f, 2.0f, 4.0f);

	// Render passes
	mRenderer.AddRenderPass<Atmosphere>(RenderPass::Normal);
}

///////////////////////////////////////////////////////////////////////////////

void WorldScene::OnDelete()
{
	LOG << "Deleting world\n";
}

///////////////////////////////////////////////////////////////////////////////