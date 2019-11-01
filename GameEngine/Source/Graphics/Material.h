#ifndef MATERIAL_H
#define MATERIAL_H

#include <Core/Array.h>

#include <Math/Vector3.h>

///////////////////////////////////////////////////////////////////////////////

class Shader;
class Texture;

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

	/* Add texture */
	void AddTexture(Texture* texture, const char* uniform);

private:
	struct MaterialTexture
	{
		Texture* mTexture;
		const char* mUniform;
	};

private:
	/* List of textures */
	Array<MaterialTexture> mTextures;

};

///////////////////////////////////////////////////////////////////////////////

#endif