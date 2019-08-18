#include <Graphics/Texture.h>

#include <Graphics/OpenGL.h>
#include <Graphics/Image.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Uint32 Texture::sCurrentBound = 0;

Texture::Texture()
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
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_2D, mID);
	sCurrentBound = mID;
}

///////////////////////////////////////////////////////////////////////////////

void Texture::SetImage(Image* image, bool mipmap)
{
	assert(sCurrentBound == mID);

	void* data = image->GetData();
	Uint32 w = image->GetWidth();
	Uint32 h = image->GetHeight();
	Uint32 c = image->GetNumChannels();

	// If image has no data, can't update texture
	if (!data) return;
	mImage = image;

	Uint32 format = GL_RED;
	if (c == 2)
		format = GL_RG;
	else if (c == 3)
		format = GL_RGB;
	else if (c == 4)
		format = GL_RGBA;
	else
		return;

	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, image->GetDataType(), data);
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
	if (!data) return;

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
}

void Texture::SetFilter(Filter filter)
{
	assert(sCurrentBound == mID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

///////////////////////////////////////////////////////////////////////////////

Image* Texture::GetImage() const
{
	return mImage;
}

///////////////////////////////////////////////////////////////////////////////