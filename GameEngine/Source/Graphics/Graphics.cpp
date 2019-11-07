#include <Graphics/Graphics.h>
#include <Graphics/OpenGL.h>

///////////////////////////////////////////////////////////////////////////////

void Graphics::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void Graphics::Clear(Uint32 flags)
{
	glClear(flags);
}

///////////////////////////////////////////////////////////////////////////////

void Graphics::Enable(Options opt)
{
	glEnable(opt);
}

void Graphics::Disable(Options opt)
{
	glDisable(opt);
}

///////////////////////////////////////////////////////////////////////////////

void Graphics::EnableCull(Options side)
{
	assert(side == Front || side == Back);
	glEnable(GL_CULL_FACE);
	glCullFace(side);
}

///////////////////////////////////////////////////////////////////////////////

void Graphics::SetDepthFunc(DepthFunc func)
{
	glDepthFunc(func);
}

///////////////////////////////////////////////////////////////////////////////