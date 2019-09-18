#include <Game/Systems/TerrainSystem.h>

#include <Scene/Scene.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TerrainSystem::TerrainSystem() :
	mTerrain		(),
	mHeightMap		(0.01f)
{

}

TerrainSystem::~TerrainSystem()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void TerrainSystem::OnInit()
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
	mTerrain.SetLodLevels(lod);
	mTerrain.SetSquareSize(3.0f);
	mTerrain.Create();

	mTerrain.SetHeightMap(&mHeightMap);
	mTerrain.SetColorMap(&mBiomeMap);

	mScene->AddRenderable(&mTerrain, true);
}

///////////////////////////////////////////////////////////////////////////////