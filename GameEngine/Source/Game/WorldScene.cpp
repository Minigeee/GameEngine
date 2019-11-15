#include <Game/WorldScene.h>

#include <Core/LogFile.h>

#include <Graphics/Atmosphere.h>

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

	ComponentMap components;
	Array<GameObjectID> ids = CreateObjects<PlayerObject>(100, &components);
	A* a = components.Get<A>();

	for (Uint32 i = 0; i < ids.Size(); ++i)
	{
		GameObjectID id = ids[i];
		a[i].mA = i * 2;
		Uint32 test = 0;
	}

	PlayerObject object = GetObject<PlayerObject>(ids[0]);
	A* comp = object.GetComponent<A>();

	// Array<PlayerObject*> objects = CreateObjects<PlayerObject>(1);
	// mRenderer.AddDynamicObject(objects[0]);
	
	// RegisterLoader<BoxLoader>();

	// Systems
	// InputSystem* system = RegisterSystem<InputSystem>();
	// system->SetMainPlayer(objects[0]);

	// RegisterSystem<TerrainSystem>();


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