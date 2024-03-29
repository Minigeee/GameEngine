#ifndef POST_PROCESS_H
#define POST_PROCESS_H

#include <Core/Array.h>

#include <Graphics/FrameBuffer.h>

#include <unordered_map>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////

class Shader;
class VertexArray;
class VertexBuffer;

class PostProcess
{
public:
	class Effect
	{
	public:
		Effect(Texture::Format fmt, Image::DataType dtype);
		virtual ~Effect();

		/* Render post processing effect to currently bound framebuffer */
		virtual void Render(VertexArray* vao) = 0;

		/* Get input framebuffer */
		FrameBuffer* GetInput() const;

	protected:
		/* Input framebuffer */
		FrameBuffer* mInput;
		/* Shader used to render effect */
		Shader* mShader;
	};

public:
	PostProcess();
	~PostProcess();

	/* Enable post processing system */
	void Enable();
	/* Returns true if system has been enabled */
	bool IsEnabled() const;

	/* Add post processing effect to queue */
	template <typename T> T* AddEffect()
	{
		assert(mIsEnabled);

		T* effect = new T(mTextureFmt, mTextureDtype);

		// Add depth texture buffer to first effect
		if (!mRenderQueue.Size())
			effect->GetInput()->AttachDepth(true);

		mRenderQueue.Push(effect);
		return effect;
	}

	/* Render all effects in queue */
	void Render(FrameBuffer* output = &FrameBuffer::Default);
	/* Render effect to an output */
	void RenderEffect(Effect* effect, FrameBuffer* output);

	/* Set default framebuffer format */
	void SetTextureFormat(Texture::Format fmt);
	/* Set default framebuffer datatype */
	void SetTextureDataType(Image::DataType dtype);

	/* Get first input buffer */
	FrameBuffer* GetInput() const;

private:
	/* Effect render queue */
	Array<Effect*> mRenderQueue;

	/* Vertex array to render quad */
	VertexArray* mVertexArray;
	/* Vertex buffer to store data */
	VertexBuffer* mVertexBuffer;
	/* Default framebuffer format */
	Texture::Format mTextureFmt;
	/* Default framebuffer data type */
	Image::DataType mTextureDtype;

	/* Returns true if Enable() has been called */
	bool mIsEnabled;
};

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Standard Effects
///////////////////////////////////////////////////////////////////////////////

class ColorAdjustment : public PostProcess::Effect
{
public:
	ColorAdjustment(Texture::Format fmt, Image::DataType dtype);
	~ColorAdjustment();

	/* Render effect */
	void Render(VertexArray* vao) override;

public:
	/* Gamma factor */
	float mGamma;
};

///////////////////////////////////////////////////////////////////////////////

#endif