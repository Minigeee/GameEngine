#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <Core/Array.h>

#include <Math/Vector3.h>

#include <Graphics/Graphics.h>
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

	/* Framebuffer bind targets */
	enum BindTarget
	{
		Read	= 0x8CA8,
		Draw	= 0x8CA9
	};

public:
	FrameBuffer();
	~FrameBuffer();

	/* Main default framebuffer */
	static FrameBuffer Default;

	/* Bind framebuffer */
	void Bind(BindTarget target = Draw);

	/* Set size of framebuffer (Call before creating attachments) */
	void SetSize(Uint32 w, Uint32 h, Uint32 d = 0);
	/* Make framebuffer multisampled (Call before creating attachments) (NOT IMPLEMENTED YET) */
	void SetMultisampled(bool ms);
	/* Create color attachment */
	Uint32 AttachColor(bool texture, const TextureOptions& options = TextureOptions());
	/* Create depth attachment */
	void AttachDepth(bool texture, const TextureOptions& options = TextureOptions());
	/* Set z-component of texture */
	void SetZValue(Uint32 z);

	/* Copy this framebuffer to another one */
	void Blit(FrameBuffer* buffer, Graphics::BufferFlags flags, Texture::Filter filter = Texture::Nearest);

	/* Get color texture */
	Texture* GetColorTexture(Uint32 index = 0) const;
	/* Get depth texture */
	Texture* GetDepthTexture() const;
	/* Get size of framebuffer */
	const Vector3u& GetSize() const;

private:
	FrameBuffer(Uint32 id);

	static Uint32 sCurrentBound[2];

private:
	/* Resolution of framebuffer */
	Vector3u mSize;

	/* Color textures */
	Array<Texture*> mColorTextures;
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