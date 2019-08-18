#ifndef TEXTURE_H
#define TEXTURE_H

#include <Graphics/GLObject.h>

///////////////////////////////////////////////////////////////////////////////

class Image;

class Texture : public GLObject
{
	TYPE_INFO(Texture);

public:
	enum Wrap
	{
		Repeat				= 0x2901,
		MirroredRepeat		= 0x8370,
		ClampToEdge			= 0x812F,
		ClampToBorder		= 0x812D
	};

	enum Filter
	{
		Linear		= 0x2601,
		Nearest		= 0x2600
	};

public:
	Texture();
	~Texture();

	/* Bind texture */
	void Bind(Uint32 slot = 0);

	/* Set image (Update texture data) */
	void SetImage(Image* image, bool mipmap = false);
	/* Set subregion of image */
	void SetSubImage(Image* image, Uint32 x, Uint32 y);
	/* Set texture wrap */
	void SetWrap(Wrap wrap);
	/* Set texture filter */
	void SetFilter(Filter filter);

	/* Get source image */
	Image* GetImage() const;

private:
	/*  Keep pointer to image */
	Image* mImage;

private:
	/* Current bounded texture */
	static Uint32 sCurrentBound;
};

///////////////////////////////////////////////////////////////////////////////

#endif