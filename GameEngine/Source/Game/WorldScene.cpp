#include <Game/WorldScene.h>

#include <Core/LogFile.h>

#include <Graphics/Atmosphere.h>
#include <Graphics/Model.h>
#include <Graphics/Material.h>
#include <Graphics/Shader.h>

#include <Graphics/Systems.h>

#include <Game/Systems/InputSystem.h>
#include <Game/Systems/BoxLoader.h>

#include <Game/Objects/PlayerObject.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

WorldScene::WorldScene() :
	mHeightMap		(0.01f)
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
	RegisterSystem<TransformMatrixSystem>();


	// Create main player
	CreateMainPlayer();
	// Create terrain
	CreateTerrain();


	mDirLight.SetDirection(0.0f, -0.5f, 1.0f);

	// Render passes
	mRenderer.SetLightingMethod<Atmosphere>();
}

///////////////////////////////////////////////////////////////////////////////

void WorldScene::OnDelete()
{
	LOG << "Deleting world\n";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void WorldScene::CreateMainPlayer()
{
	ComponentMap components;
	CreateObjects<PlayerObject>(1, &components);

	TransformComponent& t = *components.Get<TransformComponent>();
	RenderComponent& r = *components.Get<RenderComponent>();

	// Set up render component
	r.mModel = Resource<Model>::Load("Models/Box/Box.dae");

	Shader* shader = Resource<Shader>::Load("Shaders/Default.xml");

	Material* material = Resource<Material>::Create();
	material->mShader = shader;
	material->mDiffuse = Vector3f(0.8f, 0.2f, 0.2f);

	r.mModel->GetMesh(0).mMaterial = material;

	// Setup transform component
	t.mScale = 0.5f;

	mRenderer.RegisterDynamicType<PlayerObject>(r.mModel);
}

///////////////////////////////////////////////////////////////////////////////

void WorldScene::CreateTerrain()
{
	// Create height map
	mHeightMap.Generate(5, 513, 513);

	// Create biome map
	mBiomeMap.SetHeightMap(&mHeightMap);
	mBiomeMap.AddColor(Vector3f(0.651f, 0.616f, 0.325f), 0.2f);
	mBiomeMap.AddColor(Vector3f(0.247f, 0.401f, 0.179f), 1.0f);
	mBiomeMap.AddColorFilter(Vector3f(1.0f, 0.0f, 0.0f), 1, 0.05f, 0.015f);
	mBiomeMap.AddColorFilter(Vector3f(0.0f, 0.0f, 1.0f), 1, 0.05f, 0.015f);
	mBiomeMap.Generate();

	Array<float> lod(4);
	lod.Push(30.0f);
	lod.Push(100.0f);
	lod.Push(400.0f);

	mTerrain.SetSize(1000.0f);
	mTerrain.SetMaxHeight(10.0f);
	mTerrain.SetLodLevels(lod);
	mTerrain.SetSquareSize(3.0f);
	mTerrain.Create(this);

	mTerrain.SetHeightMap(&mHeightMap);
	mTerrain.SetColorMap(&mBiomeMap);


	mWater.SetAltitude(4.0f);
	mWater.SetMinDepth(1.0f);
	mWater.SetTerrain(&mTerrain);
	mWater.Create(this);
}

///////////////////////////////////////////////////////////////////////////////