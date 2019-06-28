#include <Graphics/VertexBuffer.h>
#include <Graphics/OpenGL.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Uint32 VertexBuffer::sCurrentBound = 0;

///////////////////////////////////////////////////////////////////////////////

VertexBuffer::VertexBuffer()
{
	glGenBuffers(1, &mID);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &mID);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void VertexBuffer::Bind(VertexBuffer::Target target)
{
	glBindBuffer(target, mID);
	mTarget = target;
	sCurrentBound = mID;
}

///////////////////////////////////////////////////////////////////////////////

void VertexBuffer::BufferData(const void* data, Uint32 size, Usage usage)
{
	assert(sCurrentBound == mID);

	glBufferData(mTarget, size, data, usage);
}

///////////////////////////////////////////////////////////////////////////////