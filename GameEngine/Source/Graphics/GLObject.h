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

	GLObject(GLObject&&);
	GLObject& operator=(GLObject&&);

	/* Get OpenGL ID */
	Uint32 GetID() const;

protected:
	/* OpenGL ID */
	Uint32 mID;
};

///////////////////////////////////////////////////////////////////////////////

#include <Core/TypeInfo.h>
#include <utility>

#define GL_OBJECT(x) \
TYPE_INFO(x); \
public: \
	x(x&& a) : GLObject(std::move(a)) { } \
	x& operator=(x&& a) { *this = std::move(a); }

///////////////////////////////////////////////////////////////////////////////

#endif