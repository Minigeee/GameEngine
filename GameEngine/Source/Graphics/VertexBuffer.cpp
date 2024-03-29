#include <Graphics/VertexBuffer.h>
#include <Graphics/OpenGL.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Uint32 VertexBuffer::sCurrentBound = 0;

///////////////////////////////////////////////////////////////////////////////

VertexBuffer::VertexBuffer() :
	mTarget			(Array)
{
	glGenBuffers(1, &mID);
}

VertexBuffer::~VertexBuffer()
{
	if (mID)
	{
		glDeleteBuffers(1, &mID);
		mID = 0;
	}
	else
		Uint32 a = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void VertexBuffer::Bind(VertexBuffer::Target target)
{
	glBindBuffer(target, mID);
	mTarget = target;
	sCurrentBound = mID;
}

void VertexBuffer::Bind(VertexBuffer::Target target, Uint32 index)
{
	glBindBufferBase(target, index, mID);
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

void VertexBuffer::UpdateData(const void* data, Uint32 size, Uint32 offset)
{
	assert(sCurrentBound == mID);

	glBufferSubData(mTarget, offset, size, data);
}

///////////////////////////////////////////////////////////////////////////////

void* VertexBuffer::MapWrite(Uint32 size, Uint32 opt, Uint32 offset)
{
	assert(sCurrentBound == mID);

	return glMapBufferRange(mTarget, offset, size, GL_MAP_WRITE_BIT | opt);
}

void VertexBuffer::Unmap()
{
	assert(sCurrentBound == mID);

	glUnmapBuffer(mTarget);
}

///////////////////////////////////////////////////////////////////////////////