#include <Graphics/Lights.h>
#include <Graphics/Shader.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Light::Light() :
	mDiffuse	(1.0f),
	mSpecular	(0.8f)
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DirLight::DirLight() :
	mDirection	(0.0f, -1.0f, 0.0f)
{

}

///////////////////////////////////////////////////////////////////////////////

void DirLight::Use(Shader* shader)
{
	shader->SetUniform("dirLight.diffuse", mDiffuse);
	shader->SetUniform("dirLight.specular", mSpecular);
	shader->SetUniform("dirLight.direction", mDirection);
}

///////////////////////////////////////////////////////////////////////////////

void DirLight::SetDirection(const Vector3f& dir)
{
	mDirection = Normalize(dir);
}

void DirLight::SetDirection(float x, float y, float z)
{
	mDirection = Normalize(Vector3f(x, y, z));
}

const Vector3f& DirLight::GetDirection() const
{
	return mDirection;
}

///////////////////////////////////////////////////////////////////////////////