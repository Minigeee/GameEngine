#ifndef WATER_H
#define WATER_H

#include <Scene/ObjectLoader.h>
#include <Scene/GameObject.h>

#include <Scene/Components.h>
#include <Graphics/Components.h>

///////////////////////////////////////////////////////////////////////////////

class Terrain;
class Texture;
class Material;

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
	void Create(Scene* scene, float viewDist = 150.0f, Uint32 numSquares = 8, float chunkSize = 16.0f);

	/* Set wave amplitude */
	void SetAmplitude(float a);
	/* Set noise unit size */
	void SetNoiseUnit(float size);
	/* Set wave animation speed */
	void SetWaveSpeed(float speed);
	/* Set water color */
	void SetColor(float r, float g, float b);
	/* Set minimum amount of color in water refraction */
	void SetMinColor(float min);
	/* Set density factor of water (affects murkiness of water) */
	void SetDensity(float density);
	/* Set fresnel factor (Amount of reflection vs refraction) */
	void SetFresnelFactor(float factor);
	/* Set reflection strength */
	void SetReflectStrength(float strength);
	/* Get water material */
	Material* GetMaterial() const;

private:
	/* Terrain to place water */
	Terrain* mTerrain;
	/* Water loader */
	WaterLoader* mLoader;

	/* Minimum depth */
	float mMinDepth;
	/* Altitude of water */
	float mAltitude;
	/* Amplitude of waves */
	float mAmplitude;
	/* Size of one noise unit */
	float mNoiseUnit;
	/* Speed of wave animation */
	float mWaveSpeed;
	/* Color of water */
	Vector3f mColor;
	/* Minimum amount of water in refraction */
	float mMinColor;
	/* Density factor (0 - 1) */
	float mDensity;
	/* Controls fresnel effect */
	float mFresnelFactor;
	/* Controls brightness of reflection vs color */
	float mReflectStrength;

	/* Keep pointer to model */
	Model* mModel;
};

///////////////////////////////////////////////////////////////////////////////

#endif