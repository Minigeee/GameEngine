#include <Game/WorldScene.h>

#include <Core/LogFile.h>

#include <Graphics/Atmosphere.h>

#include <Graphics/Systems.h>

#include <Game/Systems/InputSystem.h>
#include <Game/Systems/TerrainSystem.h>
#include <Game/Systems/BoxLoader.h>

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

	RegisterSystem<InputSystem>();
	// RegisterSystem<TransformMatrixSystem>();

	ComponentMap components;
	Array<GameObjectID> ids = CreateObjects<PlayerObject>(100, &components);
	
	TransformComponent* transforms = components.Get<TransformComponent>();
	for (Uint32 i = 0; i < 100; ++i)
		transforms[i].mPosition = Vector3f(2.0f * i, 10.0f, 0.0f);

	for (Uint32 i = 0; i < 100; ++i)
		QueueRemoveObject(ids[i]);


	mDirLight.SetDirection(0.0f, -0.5f, 1.0f);

	// Render passes
	mRenderer.AddRenderPass<Atmosphere>(RenderPass::Normal);
}

///////////////////////////////////////////////////////////////////////////////

void WorldScene::OnDelete()
{
	LOG << "Deleting world\n";
}

///////////////////////////////////////////////////////////////////////////////