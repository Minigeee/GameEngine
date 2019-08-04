#include <Graphics/Renderable.h>

#include <Math/Transform.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Renderable::Renderable() :
	mTransform			(1.0f),
	mPosition			(0.0f),
	mRotation			(),
	mScale				(1.0f),
	mInstanceID			(0),
	mModel				(0),
	mTransformDirty		(false)
{

}

Renderable::~Renderable()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderable::SetPosition(const Vector3f& pos)
{
	mPosition = pos;
	mTransformDirty = true;
}

void Renderable::SetPosition(float x, float y, float z)
{
	mPosition = Vector3f(x, y, z);
	mTransformDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

void Renderable::SetRotation(const Vector3f& rot)
{
	mRotation.x = fmod(rot.x, 360.0f);
	mRotation.y = fmod(rot.y, 360.0f);
	mRotation.z = fmod(rot.z, 360.0f);

	mTransformDirty = true;
}

void Renderable::SetRotation(float x, float y, float z)
{
	mRotation.x = fmod(x, 360.0f);
	mRotation.y = fmod(y, 360.0f);
	mRotation.z = fmod(z, 360.0f);

	mTransformDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

void Renderable::SetScale(float scale)
{
	mScale = scale;
	mTransformDirty = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderable::Move(const Vector3f& pos)
{
	mPosition += pos;
	mTransformDirty = true;
}

void Renderable::Move(float x, float y, float z)
{
	mPosition += Vector3f(x, y, z);
	mTransformDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

void Renderable::Rotate(const Vector3f& rot)
{
	mRotation += rot;

	mRotation.x = fmod(mRotation.x, 360.0f);
	mRotation.y = fmod(mRotation.y, 360.0f);
	mRotation.z = fmod(mRotation.z, 360.0f);

	mTransformDirty = true;
}

void Renderable::Rotate(float x, float y, float z)
{
	mRotation += Vector3f(x, y, z);

	mRotation.x = fmod(mRotation.x, 360.0f);
	mRotation.y = fmod(mRotation.y, 360.0f);
	mRotation.z = fmod(mRotation.z, 360.0f);

	mTransformDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

void Renderable::Scale(float scale)
{
	mScale = scale;
	mTransformDirty = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const Matrix4f& Renderable::GetTransform()
{
	if (mTransformDirty)
	{
		mTransform = ToTransform(mPosition, mRotation, mScale);
		mTransformDirty = false;
	}

	return mTransform;
}

///////////////////////////////////////////////////////////////////////////////

const Vector3f& Renderable::GetPosition() const
{
	return mPosition;
}

const Vector3f& Renderable::GetRotation() const
{
	return mRotation;
}

float Renderable::GetScale() const
{
	return mScale;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderable::SetModel(Model* model)
{
	mModel = model;
}

Model* Renderable::GetModel() const
{
	return mModel;
}

Uint32 Renderable::GetInstanceID() const
{
	return mInstanceID;
}

///////////////////////////////////////////////////////////////////////////////