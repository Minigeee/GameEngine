#include <Scene/Transformable.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Transformable::Transformable() :
	mPosition			(0.0f),
	mRotation			(0.0f),
	mScale				(1.0f),
	mTransformDirty		(false)
{

}

Transformable::~Transformable()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Transformable::SetPosition(const Vector3f& pos)
{
	mPosition = pos;
	mTransformDirty = true;
}

void Transformable::SetPosition(float x, float y, float z)
{
	mPosition = Vector3f(x, y, z);
	mTransformDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

void Transformable::SetRotation(const Vector3f& rot)
{
	mRotation.x = fmod(rot.x, 360.0f);
	mRotation.y = fmod(rot.y, 360.0f);
	mRotation.z = fmod(rot.z, 360.0f);

	mTransformDirty = true;
}

void Transformable::SetRotation(float x, float y, float z)
{
	mRotation.x = fmod(x, 360.0f);
	mRotation.y = fmod(y, 360.0f);
	mRotation.z = fmod(z, 360.0f);

	mTransformDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

void Transformable::SetScale(float scale)
{
	mScale = scale;
	mTransformDirty = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Transformable::Move(const Vector3f& pos)
{
	mPosition += pos;
	mTransformDirty = true;
}

void Transformable::Move(float x, float y, float z)
{
	mPosition += Vector3f(x, y, z);
	mTransformDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

void Transformable::Rotate(const Vector3f& rot)
{
	mRotation += rot;

	mRotation.x = fmod(mRotation.x, 360.0f);
	mRotation.y = fmod(mRotation.y, 360.0f);
	mRotation.z = fmod(mRotation.z, 360.0f);

	mTransformDirty = true;
}

void Transformable::Rotate(float x, float y, float z)
{
	mRotation += Vector3f(x, y, z);

	mRotation.x = fmod(mRotation.x, 360.0f);
	mRotation.y = fmod(mRotation.y, 360.0f);
	mRotation.z = fmod(mRotation.z, 360.0f);

	mTransformDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

void Transformable::Scale(float scale)
{
	mScale = scale;
	mTransformDirty = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const Vector3f& Transformable::GetPosition() const
{
	return mPosition;
}

const Vector3f& Transformable::GetRotation() const
{
	return mRotation;
}

float Transformable::GetScale() const
{
	return mScale;
}

///////////////////////////////////////////////////////////////////////////////