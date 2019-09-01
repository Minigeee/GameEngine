#include <Game/Terrain/BiomeMap.h>

#include <Resource/Resource.h>

#include <Graphics/Image.h>
#include <Graphics/Texture.h>

#include <SimplexNoise.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BiomeMap::BiomeMap() :
	mImage			(0),
	mHeightMap		(0)
{
	mImage = Resource<Image>::Create();
}

BiomeMap::~BiomeMap()
{
	if (mImage)
		Resource<Image>::Free(mImage);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void BiomeMap::SetHeightMap(Texture* map)
{
	mHeightMap = map;
}

void BiomeMap::AddColor(const Vector3f& color, float h)
{
	mBiomes[h] = color;
}

void BiomeMap::AddColorFilter(const Vector3f& color, Uint32 octaves, float freq, float amp)
{
	if (!mFilters.Capacity())
		mFilters.Resize(4);

	mFilters.Push(ColorFilter());
	ColorFilter& filter = mFilters.Back();

	// Set seed
	filter.mSeed.x = (float)(rand() / (float)RAND_MAX) * 10000.0f - 5000.0f;
	filter.mSeed.y = (float)(rand() / (float)RAND_MAX) * 10000.0f - 5000.0f;

	filter.mColor = color;
	filter.mOctaves = octaves;
	filter.mFreq = freq;
	filter.mAmp = amp;
}

///////////////////////////////////////////////////////////////////////////////

struct Biome
{
	Vector3f mColor;
	float mHeight;
};

void BiomeMap::Generate()
{
	assert(mHeightMap);

	// For quicker access while making map
	Array<Biome> biomes(mBiomes.size());
	for (auto it : mBiomes)
		biomes.Push(Biome{ it.second, it.first });

	Image* img = mHeightMap->GetImage();
	Uint32 w = img->GetWidth();
	Uint32 h = img->GetHeight();
	Uint32 size = w * h;

	// Create noise generators
	Array<SimplexNoise> generators(mFilters.Size());
	for (Uint32 i = 0; i < mFilters.Size(); ++i)
		generators.Push(SimplexNoise(mFilters[i].mFreq));


	// Generate data
	float* src = (float*)img->GetData();
	Uint8* dst = (Uint8*)malloc(size * 3);

	for (Uint32 i = 0, r = 0; r < h; ++r)
	{
		for (Uint32 c = 0; c < w; ++c, ++i)
		{
			float h = src[i];

			// Find biome level
			Uint32 lvl = 0;
			while (lvl < biomes.Size() && h > biomes[lvl].mHeight) ++lvl;

			// Set color
			Uint32 idx = 3 * i;
			lvl -= lvl < biomes.Size() ? 0 : 1;

			Vector3f color = biomes[lvl].mColor;

			// Apply color filters
			for (Uint32 f = 0; f < mFilters.Size(); ++f)
			{
				const ColorFilter& filter = mFilters[f];

				// Calculate color factor
				float d = 1.0f + generators[f].fractal(filter.mOctaves, c + filter.mSeed.x, r + filter.mSeed.y) * filter.mAmp;
				// Apply color adjustment
				color += filter.mColor * d;
			}

			dst[idx + 0] = (Uint8)(color.x * 255.0f);
			dst[idx + 1] = (Uint8)(color.y * 255.0f);
			dst[idx + 2] = (Uint8)(color.z * 255.0f);
		}
	}

	// Set image
	mImage->SetData(dst, w, h, 3);

	// Set texture
	Bind();
	SetImage(mImage);
}

///////////////////////////////////////////////////////////////////////////////