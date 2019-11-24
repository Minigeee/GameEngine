#include <Graphics/VertexArray.h>
#include <Graphics/OpenGL.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Uint32 VertexArray::sCurrentBound = 0;

///////////////////////////////////////////////////////////////////////////////

VertexArray::VertexArray() :
	mDrawMode		(Triangles)
{
	glGenVertexArrays(1, &mID);
}

VertexArray::~VertexArray()
{
	if (mID)
	{
		glDeleteVertexArrays(1, &mID);
		mID = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void VertexArray::Bind()
{
	glBindVertexArray(mID);
	sCurrentBound = mID;
}

///////////////////////////////////////////////////////////////////////////////

void VertexArray::VertexAttrib(Uint32 index, Uint32 size, Uint32 stride, Uint32 offset, Uint32 divisor)
{
	assert(sCurrentBound == mID);

	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glEnableVertexAttribArray(index);
	if (divisor)
		glVertexAttribDivisor(index, divisor);
}

///////////////////////////////////////////////////////////////////////////////

void VertexArray::SetDrawMode(DrawMode mode)
{
	mDrawMode = mode;
}

///////////////////////////////////////////////////////////////////////////////

void VertexArray::DrawArrays(Uint32 vertices, Uint32 instances, Uint32 offset)
{
	assert(sCurrentBound == mID);
	if (instances == 1)
		glDrawArrays(mDrawMode, offset, vertices);
	else
		glDrawArraysInstanced(mDrawMode, offset, vertices, instances);
}

void VertexArray::DrawElements(Uint32 vertices, Uint32 instances, Uint32 offset)
{
	assert(sCurrentBound == mID);
	if (instances == 1)
		glDrawElements(mDrawMode, vertices, GL_UNSIGNED_INT, (const void*)offset);
	else
		glDrawElementsInstanced(mDrawMode, vertices, GL_UNSIGNED_INT, (const void*)offset, instances);
}

///////////////////////////////////////////////////////////////////////////////

void VertexArray::TransformFeedback(Uint32 vertices, Uint32 offset)
{
	assert(sCurrentBound == mID);

	// Discard pixels
	glEnable(GL_RASTERIZER_DISCARD);
	// Start transform feedback mode
	glBeginTransformFeedback(mDrawMode);

	// Draw
	glDrawArrays(mDrawMode, offset, vertices);

	// Reset
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
}

///////////////////////////////////////////////////////////////////////////////