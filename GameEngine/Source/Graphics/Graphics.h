#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <Core/Array.h>
#include <Core/ObjectPool.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////

class GLObject;

class Graphics
{
public:
	enum ClearFlags
	{
		ColorBuffer		= 0x00004000,
		DepthBuffer		= 0x00000100,
		StencilBuffer	= 0x00000400
	};

	enum Options
	{
		DepthTest		= 0x0B71,
		CullFace		= 0x0B44,
		ClipPlane,
		Front			= 0x0404,
		Back			= 0x0405
	};

	enum DepthFunc
	{
		Less		= 0x0201,
		Lequal		= 0x0203,
		Greater		= 0x0204,
		Gequal		= 0x0206
	};

public:
	/* Set clear color */
	static void SetClearColor(float r, float g, float b, float a = 1.0f);
	/* Clear framebuffer */
	static void Clear(Uint32 flags = ColorBuffer | DepthBuffer);

	/* Enable graphics option */
	static void Enable(Options opt);
	/* Disable graphics option */
	static void Disable(Options opt);
	/* Enable face culling */
	static void EnableCull(Options side);

	/* Set rendering depth function */
	static void SetDepthFunc(DepthFunc func);
};

///////////////////////////////////////////////////////////////////////////////

#endif