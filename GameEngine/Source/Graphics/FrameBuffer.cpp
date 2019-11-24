#include <Graphics/FrameBuffer.h>

#include <Resource/Resource.h>

#include <Graphics/OpenGL.h>
#include <Graphics/Image.h>
#include <Graphics/Texture.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/* Used to specify up to 20 draw buffers */
Uint32 gDrawBuffers[] =
{
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,
	GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6,
	GL_COLOR_ATTACHMENT7,
	GL_COLOR_ATTACHMENT8,
	GL_COLOR_ATTACHMENT9,
	GL_COLOR_ATTACHMENT10,
	GL_COLOR_ATTACHMENT11,
	GL_COLOR_ATTACHMENT12,
	GL_COLOR_ATTACHMENT13,
	GL_COLOR_ATTACHMENT14,
	GL_COLOR_ATTACHMENT15,
	GL_COLOR_ATTACHMENT16,
	GL_COLOR_ATTACHMENT17,
	GL_COLOR_ATTACHMENT18,
	GL_COLOR_ATTACHMENT19
};

///////////////////////////////////////////////////////////////////////////////

FrameBuffer FrameBuffer::Default = FrameBuffer(0);
Uint32 FrameBuffer::sCurrentBound[2] = { 0, 0 };

FrameBuffer::FrameBuffer() :
	mSize			(1280, 720, 0),
	mColorTextures	(4),
	mDepthTexture	(0),
	mColorID		(0),
	mDepthID		(0),
	mMultisampled	(false)
{
	glGenFramebuffers(1, &mID);
}

FrameBuffer::FrameBuffer(Uint32 id) :
	mSize			(1280, 720, 0),
	mColorTextures	(4),
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
	if (mDepthTexture)
		Resource<Texture>::Free(mDepthTexture);
	if (mColorID)
		glDeleteRenderbuffers(1, &mColorID);
	if (mDepthID)
		glDeleteRenderbuffers(1, &mDepthID);

	for (Uint32 i = 0; i < mColorTextures.Size(); ++i)
		Resource<Texture>::Free(mColorTextures[i]);

	mID = 0;
	mDepthTexture = 0;
	mColorID = 0;
	mDepthID = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FrameBuffer::Bind(FrameBuffer::BindTarget target)
{
	glBindFramebuffer(target, mID);
	glViewport(0, 0, mSize.x, mSize.y);
	sCurrentBound[target - Read] = mID;
}

///////////////////////////////////////////////////////////////////////////////

void FrameBuffer::SetSize(Uint32 w, Uint32 h, Uint32 d)
{
	mSize = Vector3u(w, h, d);
}

void FrameBuffer::SetMultisampled(bool ms)
{
	mMultisampled = ms;
}

///////////////////////////////////////////////////////////////////////////////

Uint32 FrameBuffer::AttachColor(bool texture, const TextureOptions& options)
{
	assert(mID == sCurrentBound[1]);

	// Used to determine attachment number
	Uint32 index = mColorTextures.Size();

	if (texture)
	{
		// Create texture
		Texture* texture = Resource<Texture>::Create();

		// Create empty texture
		Uint32 format = options.mFormat ? options.mFormat : Texture::Rgb;
		Uint32 dtype = options.mDataType ? options.mDataType : Image::Ubyte;
		texture->SetDimensions(options.mDimensions);
		texture->Bind();
		texture->Create(format, dtype, mSize.x, mSize.y, mSize.z);
		texture->SetWrap(options.mWrap);
		texture->SetFilter(options.mFilter);

		if (options.mDimensions == Texture::_2D)
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture->GetID(), 0);
		else if (options.mDimensions == Texture::_3D)
			// Use first depth layer
			glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_3D, texture->GetID(), 0, 0);

		// Add texture to list
		mColorTextures.Push(texture);
	}
	else
	{
		// Create render buffer
		glGenRenderbuffers(1, &mColorID);
		glBindRenderbuffer(GL_RENDERBUFFER, mColorID);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, mSize.x, mSize.y);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, mColorID);
	}

	// Specify color attachments to draw
	glDrawBuffers(mColorTextures.Size(), gDrawBuffers);

	return index;
}

///////////////////////////////////////////////////////////////////////////////

void FrameBuffer::AttachDepth(bool texture, const TextureOptions& options)
{
	assert(mID == sCurrentBound[1]);

	if (texture)
	{
		// Create texture
		mDepthTexture = Resource<Texture>::Create();

		// Create empty texture
		Uint32 format = options.mFormat ? options.mFormat : Texture::Depth;
		Uint32 dtype = options.mDataType ? options.mDataType : Image::Float;
		mDepthTexture->SetDimensions(options.mDimensions);
		mDepthTexture->Bind();
		mDepthTexture->Create(format, dtype, mSize.x, mSize.y, mSize.z);
		mDepthTexture->SetWrap(options.mWrap);
		mDepthTexture->SetFilter(options.mFilter);

		if (options.mDimensions == Texture::_2D)
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture->GetID(), 0);
		else if (options.mDimensions == Texture::_3D)
			// Use first depth layer
			glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_3D, mDepthTexture->GetID(), 0, 0);
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

void FrameBuffer::SetZValue(Uint32 z)
{
	assert(mID == sCurrentBound[1]);

	for (Uint32 i = 0; i < mColorTextures.Size(); ++i)
	{
		Texture* texture = mColorTextures[i];
		texture->Bind();
		glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_3D, texture->GetID(), 0, z);
	}

	if (mDepthTexture)
	{
		mDepthTexture->Bind();
		glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_3D, mDepthTexture->GetID(), 0, z);
	}
}

///////////////////////////////////////////////////////////////////////////////

void FrameBuffer::Blit(FrameBuffer* buffer, Graphics::BufferFlags flags, Texture::Filter filter)
{
	assert(mID == sCurrentBound[0] && buffer->mID == sCurrentBound[1]);

	glBlitFramebuffer(
		0, 0, mSize.x, mSize.y,
		0, 0, buffer->mSize.x, buffer->mSize.y,
		flags, filter
	);
}

///////////////////////////////////////////////////////////////////////////////

Texture* FrameBuffer::GetColorTexture(Uint32 index) const
{
	return mColorTextures[index];
}

Texture* FrameBuffer::GetDepthTexture() const
{
	return mDepthTexture;
}

const Vector3u& FrameBuffer::GetSize() const
{
	return mSize;
}

///////////////////////////////////////////////////////////////////////////////