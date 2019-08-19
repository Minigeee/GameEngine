#ifndef TEXTURE_H
#define TEXTURE_H

#include <Graphics/GLObject.h>

///////////////////////////////////////////////////////////////////////////////

class Image;

class Texture : public GLObject
{
	GL_OBJECT(Texture);

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

	enum Format
	{
		Red		= 0x1903,
		Rg		= 0x8227,
		Rgb		= 0x1907,
		Rgba	= 0x1908,
		Depth	= 0x1902
	};

public:
	Texture();
	~Texture();

	/* Bind texture */
	void Bind(Uint32 slot = 0);

	/* Set image (Update texture data) */
	void SetImage(Image* image, bool mipmap = false, Uint32 format = 0);
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