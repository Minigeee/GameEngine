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

	enum Dimensions
	{
		_1D	= 0x0DE0,
		_2D	= 0x0DE1,
		_3D	= 0x806F
	};

public:
	Texture(Dimensions dimensions = _2D);
	~Texture();

	/* Bind texture */
	void Bind(Uint32 slot = 0);

	/* Create empty texture (Texture::Format, Image::DataType, and texture sizes) */
	void Create(Uint32 format, Uint32 dtype, Uint32 w, Uint32 h = 0, Uint32 d = 0);
	/* Set image (Update texture data) */
	void SetImage(Image* image, bool mipmap = false, Uint32 format = 0);
	/* Set subregion of image */
	void SetSubImage(Image* image, Uint32 x, Uint32 y);
	/* Set texture wrap */
	void SetWrap(Wrap wrap);
	/* Set texture filter */
	void SetFilter(Filter filter);
	/* Set number of dimensions */
	void SetDimensions(Dimensions dim);

	/* Get source image */
	Image* GetImage() const;

protected:
	/*  Keep pointer to image */
	Image* mImage;
	/* Number of dimensions */
	Dimensions mDimensions;

private:
	/* Current bounded texture */
	static Uint32 sCurrentBound;
};

///////////////////////////////////////////////////////////////////////////////

#endif