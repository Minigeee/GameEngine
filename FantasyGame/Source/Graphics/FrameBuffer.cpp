#include <Graphics/FrameBuffer.h>

#include <Resource/Resource.h>

#include <Graphics/OpenGL.h>
#include <Graphics/Image.h>
#include <Graphics/Texture.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

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

void FrameBuffer::AttachColor(bool texture, Texture::Wrap wrap, Texture::Filter filter)
{
	assert(mID == sCurrentBound);

	if (texture)
	{
		// Create texture
		mColorTexture = Resource<Texture>::Create();

		// Create empty image
		Image img;
		img.SetSize(mSize.x, mSize.y);
		img.SetNumChannels(3);
		img.SetDataType(Image::Ubyte);

		// Create empty texture
		mColorTexture->Bind();
		mColorTexture->SetImage(&img);
		mColorTexture->SetImage(0);
		mColorTexture->SetWrap(wrap);
		mColorTexture->SetFilter(filter);

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

void FrameBuffer::AttachDepth(bool texture, Texture::Wrap wrap, Texture::Filter filter)
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
		mDepthTexture->Bind();
		mDepthTexture->SetImage(&img, false, Texture::Depth);
		mDepthTexture->SetImage(0);
		mDepthTexture->SetWrap(wrap);
		mDepthTexture->SetFilter(filter);

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