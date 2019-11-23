#include <Game/Terrain/NoiseMap.h>

#include <Resource/Resource.h>

#include <Graphics/Image.h>

#include <cstdlib>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

NoiseMap::NoiseMap(float f, float a, float l, float p) :
	mGenerator		(f, a, l, p),
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
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

NormalMap::NormalMap()
{
	mImage = Resource<Image>::Create();
}

NormalMap::~NormalMap()
{

}

///////////////////////////////////////////////////////////////////////////////

void NormalMap::Generate(const NoiseMap& img, float amp)
{
	Uint32 w = img.GetImage()->GetWidth();
	Uint32 h = img.GetImage()->GetHeight();
	float* heights = (float*)img.GetImage()->GetData();

	// Allocate data
	Vector3f* data = (Vector3f*)malloc(3 * w * h * sizeof(float));

	for (Uint32 r = 0; r < h; ++r)
	{
		for (Uint32 c = 0; c < w; ++c)
		{
			Uint32 _t = (r == 0 ? r : r - 1) * w + c;
			Uint32 _b = (r == h - 1 ? r : r + 1) * w + c;
			Uint32 _l = r * w + (c == 0 ? c : c - 1);
			Uint32 _r = r * w + (c == w - 1 ? c : c + 1);

			float h_t = heights[_t] * amp;
			float h_b = heights[_b] * amp;
			float h_l = heights[_l] * amp;
			float h_r = heights[_r] * amp;

			Vector3f va = Normalize(Vector3f(1.0f, h_t - h_b, 0.0f));
			Vector3f vb = Normalize(Vector3f(0.0f, h_l - h_r, 1.0f));
			Vector3f n = Cross(vb, va);

			data[r * w + c] = n;
		}
	}

	mImage->SetData(data, w, h, 3, Image::Float);

	// Upload data
	Bind();
	SetImage(mImage);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DuDvMap::DuDvMap()
{
	mImage = Resource<Image>::Create();
}

DuDvMap::~DuDvMap()
{

}

///////////////////////////////////////////////////////////////////////////////

void DuDvMap::Generate(const NormalMap& img, float scale)
{
	Uint32 w = img.GetImage()->GetWidth();
	Uint32 h = img.GetImage()->GetHeight();
	Vector3f* normals = (Vector3f*)img.GetImage()->GetData();

	// Allocate data
	Vector3f* data = (Vector3f*)malloc(3 * w * h * sizeof(float));

	for (Uint32 r = 0; r < h; ++r)
	{
		for (Uint32 c = 0; c < w; ++c)
		{
			Uint32 _t = (r == 0 ? r : r - 1) * w + c;
			Uint32 _b = (r == h - 1 ? r : r + 1) * w + c;
			Uint32 _l = r * w + (c == 0 ? c : c - 1);
			Uint32 _r = r * w + (c == w - 1 ? c : c + 1);

			Vector3f du = normals[_r] * scale - normals[_l] * scale;
			Vector3f dv = normals[_t] * scale - normals[_b] * scale;

			data[r * w + c] = (du + dv) * 0.5f;
		}
	}

	mImage->SetData(data, w, h, 3, Image::Float);

	// Upload data
	Bind();
	SetImage(mImage);
}

///////////////////////////////////////////////////////////////////////////////