#include <Graphics/Texture.h>

#include <Graphics/OpenGL.h>
#include <Graphics/Image.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Uint32 Texture::sCurrentBound = 0;

Texture::Texture(Dimensions dimensions) :
	mDimensions		(dimensions)
{
	glGenTextures(1, &mID);
}

Texture::~Texture()
{
	if (mID)
		glDeleteTextures(1, &mID);
	mID = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Texture::Bind(Uint32 slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(mDimensions, mID);
	sCurrentBound = mID;
}

///////////////////////////////////////////////////////////////////////////////

Uint32 GetInternalFormat(Uint32 format, Uint32 dtype)
{
	Uint32 fmt = format;
	if (dtype == Image::Ushort)
	{
		if (format == Texture::Red)
			fmt = GL_R16F;
		else if (format == Texture::Rg)
			fmt = GL_RG16F;
		else if (format == Texture::Rgb)
			fmt = GL_RGB16F;
		else if (format == Texture::Rgba)
			fmt = GL_RGBA16F;
	}
	else if (dtype == Image::Float)
	{
		if (format == Texture::Red)
			fmt = GL_R32F;
		else if (format == Texture::Rg)
			fmt = GL_RG32F;
		else if (format == Texture::Rgb)
			fmt = GL_RGB32F;
		else if (format == Texture::Rgba)
			fmt = GL_RGBA32F;
	}

	return fmt;
}

///////////////////////////////////////////////////////////////////////////////

void Texture::Create(Uint32 format, Uint32 dtype, Uint32 w, Uint32 h, Uint32 d)
{
	assert(sCurrentBound == mID);

	Uint32 internalFmt = GetInternalFormat(format, dtype);

	if (mDimensions == _2D)
		glTexImage2D(GL_TEXTURE_2D, 0, internalFmt, w, h, 0, format, dtype, 0);
	else if (mDimensions == _3D)
		glTexImage3D(GL_TEXTURE_3D, 0, internalFmt, w, h, d, 0, format, dtype, 0);
}

///////////////////////////////////////////////////////////////////////////////

void Texture::SetImage(Image* image, bool mipmap, Uint32 fmt)
{
	mImage = image;
	// This function only usable with 2D images
	if (!image || mDimensions != _2D) return;

	assert(sCurrentBound == mID);

	void* data = image->GetData();
	Uint32 w = image->GetWidth();
	Uint32 h = image->GetHeight();
	Uint32 c = image->GetNumChannels();

	Uint32 format = fmt;
	if (!format)
	{
		if (c == 1)
			format = GL_RED;
		else if (c == 2)
			format = GL_RG;
		else if (c == 3)
			format = GL_RGB;
		else if (c == 4)
			format = GL_RGBA;
		else
			return;
	}

	// Get internal format
	Uint32 dtype = image->GetDataType();
	Uint32 internalFmt = GetInternalFormat(format, dtype);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Buffer data
	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, dtype, data);

	if (mipmap)
		glGenerateMipmap(GL_TEXTURE_2D);
}

///////////////////////////////////////////////////////////////////////////////

void Texture::SetSubImage(Image* image, Uint32 x, Uint32 y)
{
	assert(sCurrentBound == mID);

	void* data = image->GetData();
	Uint32 w = image->GetWidth();
	Uint32 h = image->GetHeight();
	Uint32 c = image->GetNumChannels();

	// If image has no data, can't update texture
	// This function only usable with 2D images
	if (!data || mDimensions != _2D) return;

	Uint32 format = GL_RED;
	if (c == 2)
		format = GL_RG;
	else if (c == 3)
		format = GL_RGB;
	else if (c == 4)
		format = GL_RGBA;
	else
		return;

	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, format, image->GetDataType(), data);
}

///////////////////////////////////////////////////////////////////////////////

void Texture::SetWrap(Wrap wrap)
{
	assert(sCurrentBound == mID);
	glTexParameteri(mDimensions, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(mDimensions, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(mDimensions, GL_TEXTURE_WRAP_R, wrap);
}

void Texture::SetFilter(Filter filter)
{
	assert(sCurrentBound == mID);
	glTexParameteri(mDimensions, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(mDimensions, GL_TEXTURE_MAG_FILTER, filter);
}

///////////////////////////////////////////////////////////////////////////////

void Texture::SetDimensions(Texture::Dimensions dim)
{
	mDimensions = dim;
	if (sCurrentBound == mID)
		glBindTexture(mDimensions, mID);
}

///////////////////////////////////////////////////////////////////////////////

Image* Texture::GetImage() const
{
	return mImage;
}

///////////////////////////////////////////////////////////////////////////////