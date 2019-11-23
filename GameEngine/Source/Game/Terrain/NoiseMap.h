#ifndef NOISE_MAP_H
#define NOISE_MAP_H

#include <Math/Vector2.h>
#include <Math/Vector3.h>

#include <Graphics/Texture.h>

#include <SimplexNoise.h>

///////////////////////////////////////////////////////////////////////////////

class Image;

class NoiseMap : public Texture
{
public:
	NoiseMap(
		float frequency = 1.0f,
		float amplitude = 1.0f,
		float lacunarity = 2.0f,
		float persistence = 0.5f);
	~NoiseMap();

	/* Generate noise map and upload */
	void Generate(Uint32 octaves, Uint32 w, Uint32 h);

private:
	/* Noise generator */
	SimplexNoise mGenerator;

	/* Float seed */
	Vector2f mSeed;
};

///////////////////////////////////////////////////////////////////////////////

class NormalMap : public Texture
{
public:
	NormalMap();
	~NormalMap();

	void Generate(const NoiseMap& img, float amp);
};

///////////////////////////////////////////////////////////////////////////////

class DuDvMap : public Texture
{
public:
	DuDvMap();
	~DuDvMap();

	void Generate(const NormalMap& img, float scale);
};

///////////////////////////////////////////////////////////////////////////////

#endif