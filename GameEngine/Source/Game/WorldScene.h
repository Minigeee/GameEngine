#ifndef WORLD_SCENE_H
#define WORLD_SCENE_H

#include <Scene/Scene.h>

#include <Graphics/Terrain.h>
#include <Graphics/Water.h>
#include <Game/Terrain/NoiseMap.h>
#include <Game/Terrain/BiomeMap.h>

///////////////////////////////////////////////////////////////////////////////

class WorldScene : public Scene
{
public:
	WorldScene();
	~WorldScene();

private:
	/* Setup world */
	void OnCreate() override;
	/* Clean up resources */
	void OnDelete() override;

	/* Create main player */
	void CreateMainPlayer();
	/* Create terrain */
	void CreateTerrain();

private:
	Terrain mTerrain;
	NoiseMap mHeightMap;
	BiomeMap mBiomeMap;

	Water mWater;
	NormalMap mWaterNormals;
	DuDvMap mWaterDuDv;
};

///////////////////////////////////////////////////////////////////////////////

#endif