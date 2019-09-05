#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <Math/Vector4.h>

#include <Graphics/FrameBuffer.h>

///////////////////////////////////////////////////////////////////////////////

/* Defines render pass options */
struct RenderPass
{
	enum Type
	{
		None = 0,
		Normal = 1 << 0,
		Reflect = 1 << 1,
		Refract = 1 << 2,
		Shadow = 1 << 3
	};

	RenderPass() = default;
	RenderPass(Type type, FrameBuffer* output, const Vector4f& refl = Vector4f(0.0f)):
		mType				(type),
		mOutput				(output),
		mReflectionPlane	(refl)
	{ }

	/* Render pass type */
	Type mType;
	/* Ouptput framebuffer */
	FrameBuffer* mOutput;

	/* Reflection plane */
	Vector4f mReflectionPlane;
};

///////////////////////////////////////////////////////////////////////////////

#endif