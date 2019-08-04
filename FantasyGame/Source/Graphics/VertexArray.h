#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <Core/TypeInfo.h>

#include <Graphics/GLObject.h>

///////////////////////////////////////////////////////////////////////////////

class VertexArray : public GLObject
{
	GL_OBJECT(VertexArray);

public:
	VertexArray();
	~VertexArray();

	/* Bind vertex array */
	void Bind();

	/* Enable vertex attrib */
	void VertexAttrib(Uint32 index, Uint32 size, Uint32 stride = 0, Uint32 offset = 0, Uint32 divisor = 0);

	/* Draw from buffers */
	void DrawArrays(Uint32 vertices, Uint32 instances = 1, Uint32 offset = 0);
	/* Draw from element buffer */
	void DrawElements(Uint32 vertices, Uint32 instances = 1, Uint32 offset = 0);

private:
	/* Current bound VAO */
	static Uint32 sCurrentBound;
};

///////////////////////////////////////////////////////////////////////////////

#endif