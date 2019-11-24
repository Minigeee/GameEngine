#ifndef SHADER_H
#define SHADER_H

#include <Core/TypeInfo.h>
#include <Core/Array.h>

#include <Math/Vector4.h>
#include <Math/Matrix4.h>

#include <Graphics/GLObject.h>

#include <Resource/Loadable.h>

///////////////////////////////////////////////////////////////////////////////

struct Uniform
{
	Uniform() = default;
	Uniform(const char* name);

	enum Type
	{
		Int,
		Float,
		Vec2,
		Vec3,
		Vec4,
		Mat2,
		Mat3,
		Mat4
	};

	/* Uniform name */
	const char* mName;
	/* Storage for uniform variable */
	float mVariable[16];
	/* Uniform type */
	Uint16 mType;

	/* True if uniform value has changed */
	bool mHasChanged;
};

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
	/* Update all uniforms */
	void ApplyUniforms();

private:
	/* Load shader code from file */
	Uint32 LoadShader(const char* fname, Uint32 type);
	/* Find uniform object */
	Uniform& FindUniform(const char* name);

private:
	/* Current bound shader */
	static Uint32 sCurrentBound;

private:
	/* Uniform map */
	Array<Uniform> mUniforms;
};

///////////////////////////////////////////////////////////////////////////////

#endif