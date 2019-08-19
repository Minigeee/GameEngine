#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <Math/Vector2.h>

#include <Graphics/GLObject.h>
#include <Graphics/Texture.h>

///////////////////////////////////////////////////////////////////////////////

class FrameBuffer : public GLObject
{
	GL_OBJECT(FrameBuffer);

public:
	FrameBuffer();
	~FrameBuffer();

	/* Main default framebuffer */
	static FrameBuffer Default;

	/* Bind framebuffer */
	void Bind();

	/* Set size of framebuffer (Call before creating attachments) */
	void SetSize(Uint32 w, Uint32 h);
	/* Make framebuffer multisampled (Call before creating attachments) (NOT IMPLEMENTED YET) */
	void SetMultisampled(bool ms);
	/* Create color attachment */
	void AttachColor(bool texture, Texture::Wrap wrap = Texture::ClampToEdge, Texture::Filter filter = Texture::Linear);
	/* Create depth attachment */
	void AttachDepth(bool texture, Texture::Wrap wrap = Texture::ClampToEdge, Texture::Filter filter = Texture::Linear);

	/* Get color texture */
	Texture* GetColorTexture() const;
	/* Get depth texture */
	Texture* GetDepthTexture() const;

private:
	FrameBuffer(Uint32 id);

	static Uint32 sCurrentBound;

private:
	/* Resolution of framebuffer */
	Vector2u mSize;

	/* Color texture */
	Texture* mColorTexture;
	/* Depth texture */
	Texture* mDepthTexture;

	/* Color renderbuffer */
	Uint32 mColorID;
	/* Depth renderbuffer */
	Uint32 mDepthID;

	/* Should framebuffer be multisampled */
	bool mMultisampled;
};

///////////////////////////////////////////////////////////////////////////////

#endif