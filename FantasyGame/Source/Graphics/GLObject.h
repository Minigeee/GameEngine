#ifndef GL_OBJECT_H
#define GL_OBJECT_H

#include <Core/DataTypes.h>

///////////////////////////////////////////////////////////////////////////////

class GLObject
{
public:
	GLObject();
	virtual ~GLObject();

	/* Delete copy operations */
	GLObject(const GLObject&) = delete;
	GLObject& operator=(const GLObject&) = delete;

	/* Get OpenGL ID */
	Uint32 GetID() const;

protected:
	/* OpenGL ID */
	Uint32 mID;
};

///////////////////////////////////////////////////////////////////////////////

#endif