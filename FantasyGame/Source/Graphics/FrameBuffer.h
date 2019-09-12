#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <Math/Vector3.h>

#include <Graphics/GLObject.h>
#include <Graphics/Texture.h>
#include <Graphics/Image.h>

///////////////////////////////////////////////////////////////////////////////

class FrameBuffer : public GLObject
{
	GL_OBJECT(FrameBuffer);

public:
	struct TextureOptions
	{
		TextureOptions(
			Texture::Wrap wrap = Texture::ClampToEdge,
			Texture::Filter filter = Texture::Linear,
			Uint32 dtype = 0,
			Uint32 format = 0,
			Texture::Dimensions dims = Texture::_2D) :
			mWrap		(wrap),
			mFilter		(filter),
			mDataType	(dtype),
			mFormat		(format),
			mDimensions	(dims)
		{ }

		/* Wrap option */
		Texture::Wrap mWrap;
		/* Filter option */
		Texture::Filter mFilter;
		/* Texture data type */
		Uint32 mDataType;
		/* Number of dimensions */
		Texture::Dimensions mDimensions;
		/* Format option */
		Uint32 mFormat;
	};

public:
	FrameBuffer();
	~FrameBuffer();

	/* Main default framebuffer */
	static FrameBuffer Default;

	/* Bind framebuffer */
	void Bind();

	/* Set size of framebuffer (Call before creating attachments) */
	void SetSize(Uint32 w, Uint32 h, Uint32 d = 0);
	/* Make framebuffer multisampled (Call before creating attachments) (NOT IMPLEMENTED YET) */
	void SetMultisampled(bool ms);
	/* Create color attachment */
	void AttachColor(bool texture, const TextureOptions& options = TextureOptions());
	/* Create depth attachment */
	void AttachDepth(bool texture, const TextureOptions& options = TextureOptions());
	/* Set z-component of texture */
	void SetZValue(Uint32 z);

	/* Get color texture */
	Texture* GetColorTexture() const;
	/* Get depth texture */
	Texture* GetDepthTexture() const;
	/* Get size of framebuffer */
	const Vector3u& GetSize() const;

private:
	FrameBuffer(Uint32 id);

	static Uint32 sCurrentBound;

private:
	/* Resolution of framebuffer */
	Vector3u mSize;

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