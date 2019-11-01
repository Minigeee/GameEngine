#include <Game/Terrain/NoiseMap.h>

#include <Resource/Resource.h>

#include <Graphics/Image.h>

#include <cstdlib>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

NoiseMap::NoiseMap(float f, float a, float l, float p) :
	mGenerator		(f, a, l, p),
	mImage			(0),
	mSeed			(0.0f)
{
	mImage = Resource<Image>::Create();
	mSeed.x = (float)(rand() / (float)RAND_MAX) * 10000.0f - 5000.0f;
	mSeed.y = (float)(rand() / (float)RAND_MAX) * 10000.0f - 5000.0f;
}

NoiseMap::~NoiseMap()
{
	if (mImage)
		Resource<Image>::Free(mImage);
	mImage = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void NoiseMap::Generate(Uint32 octaves, Uint32 w, Uint32 h)
{
	float* data = (float*)malloc(w * h * sizeof(float));

	// Generate image
	for (int r = 0; r < h; ++r)
	{
		for (int c = 0; c < w; ++c)
		{
			float h = mGenerator.fractal(octaves, c + mSeed.x, r + mSeed.y) * 0.5f + 0.5f;
			data[r * w + c] = h;
		}
	}

	mImage->SetData(data, w, h, 1, Image::Float);

	// Upload data
	Bind();
	SetImage(mImage);
}

///////////////////////////////////////////////////////////////////////////////