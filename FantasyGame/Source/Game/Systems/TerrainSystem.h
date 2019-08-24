#ifndef TERRAIN_SYSTEM_H
#define TERRAIN_SYSTEM_H

#include <Scene/GameSystem.h>

#include <Graphics/Terrain.h>

#include <Game/Terrain/NoiseMap.h>

///////////////////////////////////////////////////////////////////////////////

class TerrainSystem : public GameSystem
{
	TYPE_INFO(TerrainSystem);

public:
	TerrainSystem();
	~TerrainSystem();

private:
	void OnInit() override;

private:
	/* Terrain object */
	Terrain mTerrain;
	/* Height map for terrain */
	NoiseMap mHeightMap;
};

///////////////////////////////////////////////////////////////////////////////

#endif