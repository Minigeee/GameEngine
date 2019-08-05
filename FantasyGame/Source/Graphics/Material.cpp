#include <Graphics/Material.h>
#include <Graphics/Shader.h>

///////////////////////////////////////////////////////////////////////////////

Material::Material() :
	mDiffuse		(1.0f),
	mSpecular		(0.5f),
	mSpecFactor		(16.0f),

	mShader			(0)
{

}

///////////////////////////////////////////////////////////////////////////////

void Material::Use()
{
	mShader->SetUniform("material.diffuse", mDiffuse);
	mShader->SetUniform("material.specular", mSpecular);
	mShader->SetUniform("material.specFactor", mSpecFactor);
}

///////////////////////////////////////////////////////////////////////////////