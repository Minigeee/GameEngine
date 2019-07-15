#ifndef MESH_H
#define MESH_H

#include <Core/DataTypes.h>
#include <Math/BoundingBox.h>

///////////////////////////////////////////////////////////////////////////////

class VertexArray;

/* Container for VAO, vertices, material, etc. */
class Mesh
{
public:
	Mesh();

	/* Vertex array to draw mesh */
	VertexArray* mVertexArray;
	/* Number of vertices in mesh */
	Uint32 mNumVertices;
	/* Bounding box */
	BoundingBox mBoundingBox;
};

///////////////////////////////////////////////////////////////////////////////

#endif