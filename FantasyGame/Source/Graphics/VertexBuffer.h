#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <Core/TypeInfo.h>

#include <Graphics/GLObject.h>

///////////////////////////////////////////////////////////////////////////////

class VertexBuffer : public GLObject
{
	TYPE_INFO(VertexBuffer);

public:
	enum Target
	{
		Array		= 0x8892,
		Element		= 0x8893
	};

	enum Usage
	{
		Static		= 0x88E4,
		Dynamic		= 0x88E8,
		Stream		= 0x88E0
	};

public:
	VertexBuffer();
	~VertexBuffer();

	/* Bind buffer */
	void Bind(Target target);

	/* Send data for first time */
	void BufferData(const void* data, Uint32 size, Usage usage);

private:
	/* Current bound buffer */
	static Uint32 sCurrentBound;

	/* Current buffer target */
	Target mTarget;
};

///////////////////////////////////////////////////////////////////////////////

#endif