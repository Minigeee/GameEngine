#ifndef WATER_H
#define WATER_H

#include <Scene/ObjectLoader.h>
#include <Scene/GameObject.h>

#include <Scene/Components.h>
#include <Graphics/Components.h>

///////////////////////////////////////////////////////////////////////////////

class Terrain;

///////////////////////////////////////////////////////////////////////////////

class WaterLoader : public ObjectLoader
{
	TYPE_INFO(WaterLoader);

public:
	WaterLoader();
	~WaterLoader();

	/* Set water chunk model */
	void SetModel(Model* model);
	/* Set water map */
	void SetWaterMap(const Array<bool>& map, Uint32 size);

private:
	void OnInit() override;
	void OnChunkEnter(ObjectChunk& chunk) override;
	void OnChunkLeave(ObjectChunk& chunk) override;

private:
	/* Water chunk model */
	Model* mModel;

	/* Which chunks have water */
	Array<bool> mWaterMap;
	/* Map size */
	Uint32 mSize;
};

///////////////////////////////////////////////////////////////////////////////

class WaterChunk : public GameObject
{
	GAME_OBJECT(WaterChunk);

	REGISTER_COMPONENTS(
		TransformComponent,
		RenderComponent
	);

	REGISTER_TAGS();
};

///////////////////////////////////////////////////////////////////////////////

class Water
{
public:
	Water();
	~Water();

	/* Set terrain to place water chunks */
	void SetTerrain(Terrain* terrain);
	/* Set minimum depth of water */
	void SetMinDepth(float min);
	/* Set altitude level of water */
	void SetAltitude(float h);
	/* Create water */
	void Create(Scene* scene, float viewDist = 150.0f, float chunkSize = 16.0f);

private:
	/* Terrain to place water */
	Terrain* mTerrain;
	/* Water loader */
	WaterLoader* mLoader;

	/* Minimum depth */
	float mMinDepth;
	/* Altitude of water */
	float mAltitude;
};

///////////////////////////////////////////////////////////////////////////////

#endif