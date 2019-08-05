#ifndef MATERIAL_H
#define MATERIAL_H

#include <Math/Vector3.h>

///////////////////////////////////////////////////////////////////////////////

class Shader;

class Material
{
public:
	Material();

	/* Apply material to shader */
	void Use();

	/* Diffuse color */
	Vector3f mDiffuse;
	/* Specular color */
	Vector3f mSpecular;
	/* Specularity factor (shininess) */
	float mSpecFactor;

	/* Shader to render with */
	Shader* mShader;
};

///////////////////////////////////////////////////////////////////////////////

#endif