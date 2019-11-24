#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <Core/TypeInfo.h>

#include <Graphics/GLObject.h>

///////////////////////////////////////////////////////////////////////////////

class VertexBuffer : public GLObject
{
	GL_OBJECT(VertexBuffer);

public:
	enum Target
	{
		Array				= 0x8892,
		Element				= 0x8893,
		TransformFeedback	= 0x8C8E
	};

	enum Usage
	{
		Static		= 0x88E4,
		Dynamic		= 0x88E8,
		Stream		= 0x88E0
	};

	enum MapOptions
	{
		Unsynchronized		= 0x0020,
		InvalidateBuffer	= 0x0008,
		InvalidateRange		= 0x0004
	};

public:
	VertexBuffer();
	~VertexBuffer();

	/* Bind buffer */
	void Bind(Target target);
	/* Bind buffer to index */
	void Bind(Target target, Uint32 index);

	/* Send data for first time */
	void BufferData(const void* data, Uint32 size, Usage usage);
	/* Update with new data */
	void UpdateData(const void* data, Uint32 size, Uint32 offset = 0);
	/* Map buffer for write operations */
	void* MapWrite(Uint32 size, Uint32 opt, Uint32 offset = 0);
	/* Unmap all mapped ranges */
	void Unmap();

private:
	/* Current bound buffer */
	static Uint32 sCurrentBound;

	/* Current buffer target */
	Target mTarget;
};

///////////////////////////////////////////////////////////////////////////////

#endif