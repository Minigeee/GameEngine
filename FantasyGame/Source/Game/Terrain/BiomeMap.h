#ifndef BIOME_MAP_H
#define BIOME_MAP_H

#include <Core/Array.h>

#include <Graphics/Texture.h>

#include <Math/Vector3.h>

#include <map>

///////////////////////////////////////////////////////////////////////////////

class Image;

class BiomeMap : public Texture
{
	TYPE_INFO(BiomeMap);

public:
	BiomeMap();
	~BiomeMap();

	/* Set base height map */
	void SetHeightMap(Texture* map);
	/* Add biome at height level */
	void AddColor(const Vector3f& color, float h);
	/* Add color filter */
	void AddColorFilter(const Vector3f& color, Uint32 octaves, float freq, float amp = 0.1f);

	/* Generate biome map (With parameters for discoloration map) */
	void Generate();

private:
	/* Color filter info */
	struct ColorFilter
	{
		/* Color filter */
		Vector3f mColor;
		/* Number of octaves */
		Uint32 mOctaves;
		/* Noise frequency */
		float mFreq;
		/* Color amplitude */
		float mAmp;

		/* Random seed */
		Vector2f mSeed;
	};

private:
	/* List of color filters to apply */
	Array<ColorFilter> mFilters;
	/* Sorted map of biomes */
	std::map<float, Vector3f> mBiomes;

	/* Source image */
	Image* mImage;
	/* Height map */
	Texture* mHeightMap;
};

///////////////////////////////////////////////////////////////////////////////

#endif