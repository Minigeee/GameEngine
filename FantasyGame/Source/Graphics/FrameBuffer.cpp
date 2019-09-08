#include <Graphics/FrameBuffer.h>

#include <Resource/Resource.h>

#include <Graphics/OpenGL.h>
#include <Graphics/Image.h>
#include <Graphics/Texture.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FrameBuffer FrameBuffer::Default = FrameBuffer(0);
Uint32 FrameBuffer::sCurrentBound = 0;

FrameBuffer::FrameBuffer() :
	mSize			(1280, 720),
	mColorTexture	(0),
	mDepthTexture	(0),
	mColorID		(0),
	mDepthID		(0),
	mMultisampled	(false)
{
	glGenFramebuffers(1, &mID);
}

FrameBuffer::FrameBuffer(Uint32 id) :
	mSize			(1280, 720),
	mColorTexture	(0),
	mDepthTexture	(0),
	mColorID		(0),
	mDepthID		(0),
	mMultisampled	(false)
{
	mID = id;
}

FrameBuffer::~FrameBuffer()
{
	if (mID)
		glDeleteFramebuffers(1, &mID);
	mID = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mID);
	glViewport(0, 0, mSize.x, mSize.y);
	sCurrentBound = mID;
}

///////////////////////////////////////////////////////////////////////////////

void FrameBuffer::SetSize(Uint32 w, Uint32 h)
{
	mSize = Vector2u(w, h);
}

void FrameBuffer::SetMultisampled(bool ms)
{
	mMultisampled = ms;
}

///////////////////////////////////////////////////////////////////////////////

void FrameBuffer::AttachColor(bool texture, const TextureOptions& options)
{
	assert(mID == sCurrentBound);

	if (texture)
	{
		// Create texture
		mColorTexture = Resource<Texture>::Create();

		// Create empty image
		Image img;
		img.SetSize(mSize.x, mSize.y);
		img.SetDataType(Image::Ubyte);

		// Create empty texture
		Uint32 format = options.mFormat ? options.mFormat : Texture::Rgb;
		mColorTexture->Bind();
		mColorTexture->SetImage(&img, false, format);
		mColorTexture->SetImage(0);
		mColorTexture->SetWrap(options.mWrap);
		mColorTexture->SetFilter(options.mFilter);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture->GetID(), 0);
	}
	else
	{
		// Create render buffer
		glGenRenderbuffers(1, &mColorID);
		glBindRenderbuffer(GL_RENDERBUFFER, mColorID);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, mSize.x, mSize.y);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mColorID);
	}
}

///////////////////////////////////////////////////////////////////////////////

void FrameBuffer::AttachDepth(bool texture, const TextureOptions& options)
{
	assert(mID == sCurrentBound);

	if (texture)
	{
		// Create texture
		mDepthTexture = Resource<Texture>::Create();

		// Create empty image
		Image img;
		img.SetSize(mSize.x, mSize.y);
		img.SetDataType(Image::Float);

		// Create empty texture
		Uint32 format = options.mFormat ? options.mFormat : Texture::Depth;
		mDepthTexture->Bind();
		mDepthTexture->SetImage(&img, false, format);
		mDepthTexture->SetImage(0);
		mDepthTexture->SetWrap(options.mWrap);
		mDepthTexture->SetFilter(options.mFilter);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture->GetID(), 0);
	}
	else
	{
		// Create render buffer
		glGenRenderbuffers(1, &mDepthID);
		glBindRenderbuffer(GL_RENDERBUFFER, mDepthID);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mSize.x, mSize.y);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthID);
	}
}

///////////////////////////////////////////////////////////////////////////////

Texture* FrameBuffer::GetColorTexture() const
{
	return mColorTexture;
}

Texture* FrameBuffer::GetDepthTexture() const
{
	return mDepthTexture;
}

///////////////////////////////////////////////////////////////////////////////