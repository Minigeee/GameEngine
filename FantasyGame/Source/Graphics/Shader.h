#ifndef SHADER_H
#define SHADER_H

#include <Core/TypeInfo.h>

#include <Graphics/GLObject.h>

///////////////////////////////////////////////////////////////////////////////

class Shader : public GLObject
{
	TYPE_INFO(Shader);

public:
	Shader();
	virtual ~Shader();

	/* Load shader from XML file */
	bool Load(const char* fname);

	/* Bind shader */
	void Bind();

private:
	/* Load shader code from file */
	Uint32 LoadShader(const char* fname, Uint32 type);

private:
	/* Current bound shader */
	static Uint32 sCurrentBound;
};

///////////////////////////////////////////////////////////////////////////////

#endif