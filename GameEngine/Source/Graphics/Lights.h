#ifndef LIGHTS_H
#define LIGHTS_H

#include <Math/Vector3.h>

///////////////////////////////////////////////////////////////////////////////

class Shader;

class Light
{
public:
	Light();

	/* Diffuse color */
	Vector3f mDiffuse;
	/* Specular color */
	Vector3f mSpecular;
};

///////////////////////////////////////////////////////////////////////////////

class DirLight : public Light
{
public:
	DirLight();

	/* Apply light to shader */
	void Use(Shader* shader);

	/* Use setter to automatically normalize */
	void SetDirection(const Vector3f& dir);
	/* Use setter to automatically normalize */
	void SetDirection(float x, float y, float z);
	/* Get direction */
	const Vector3f& GetDirection() const;

private:
	/* Direction of light */
	Vector3f mDirection;
};

///////////////////////////////////////////////////////////////////////////////

#endif