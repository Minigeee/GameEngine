#ifndef SHADER_H
#define SHADER_H

#include <Core/TypeInfo.h>

#include <Math/Vector4.h>
#include <Math/Matrix4.h>

#include <Graphics/GLObject.h>

#include <Resource/Loadable.h>

///////////////////////////////////////////////////////////////////////////////

class Shader :
	public GLObject,
	public Loadable
{
	GL_OBJECT(Shader);

public:
	Shader();
	virtual ~Shader();

	/* Load shader from XML file */
	bool Load(const char* fname);

	/* Bind shader */
	void Bind();

	/* Set shader uniform */
	void SetUniform(const char* name, int val);
	/* Set shader uniform */
	void SetUniform(const char* name, float val);
	/* Set shader uniform */
	void SetUniform(const char* name, const Vector2f& val);
	/* Set shader uniform */
	void SetUniform(const char* name, const Vector3f& val);
	/* Set shader uniform */
	void SetUniform(const char* name, const Vector4f& val);
	/* Set shader uniform */
	void SetUniform(const char* name, const Matrix2f& val);
	/* Set shader uniform */
	void SetUniform(const char* name, const Matrix3f& val);
	/* Set shader uniform */
	void SetUniform(const char* name, const Matrix4f& val);

private:
	/* Load shader code from file */
	Uint32 LoadShader(const char* fname, Uint32 type);

private:
	/* Current bound shader */
	static Uint32 sCurrentBound;
};

///////////////////////////////////////////////////////////////////////////////

#endif