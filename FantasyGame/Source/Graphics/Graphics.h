#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <Core/Array.h>
#include <Core/ObjectPool.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////

class GLObject;

class Graphics
{
private:
	enum ClearFlags
	{
		ColorBuffer		= 0x00004000,
		DepthBuffer		= 0x00000100,
		StencilBuffer	= 0x00000400
	};

public:
	/* Set clear color */
	static void SetClearColor(float r, float g, float b, float a = 1.0f);
	/* Clear framebuffer */
	static void Clear(Uint32 flags = ColorBuffer | DepthBuffer);
};

///////////////////////////////////////////////////////////////////////////////

#endif